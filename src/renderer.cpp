#include "renderer.h"
#include <algorithm>

//Clears the screen at start of each frame
void beginFrame(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 230, 225, 215, 255); //Light background
    SDL_RenderClear(renderer);
}

//Renders the grid dots for canvas background
void renderCanvas(SDL_Renderer* renderer, const Canvas& canvas, int windowWidth, int windowHeight) {

    float alpha = SDL_clamp(canvas.zoom * 0.5f, 0.15f, 0.5f); //Fades grid at low zoom
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, (Uint8)(alpha * 255));

    const float spacing = 100.0f; //Grid spacing

    float left = canvas.cameraX;
    float right = canvas.cameraX + (windowWidth / canvas.zoom);
    float top = canvas.cameraY;
    float bottom = canvas.cameraY + (windowHeight / canvas.zoom);

    float startX = floorf(left / spacing) * spacing;
    float startY = floorf(top / spacing) * spacing;

    //Draws grid dots
    for (float x = startX; x < right; x += spacing) {
        for (float y = startY; y < bottom; y += spacing) {

            float sx = (x - canvas.cameraX) * canvas.zoom;
            float sy = (y - canvas.cameraY) * canvas.zoom;

            float size = 2.0f;

            SDL_FRect dot = {sx - size * 0.5f, sy - size * 0.5f, size, size};

            SDL_RenderFillRect(renderer, &dot);
        }
    }
}

//Renders nodes on canvas
void renderNodes(SDL_Renderer* renderer, TTF_Font* font, EntityManager& em, const Canvas& canvas, float windowWidth, float windowHeight) {
    const float margin = 200.0f; //Extra margin for smooth visibility on camera move

    float left = canvas.cameraX - margin;
    float top = canvas.cameraY - margin;
    float right = canvas.cameraX + (windowWidth / canvas.zoom) + margin;
    float bottom = canvas.cameraY + (windowHeight / canvas.zoom) + margin;


    for (auto entity : em.getEntities()) {

        auto* pos = em.getComponent<PositionComponent>(entity);
        auto* size = em.getComponent<sizeComponent>(entity);
        auto* type = em.getComponent<NodeTypeComponent>(entity);
        auto* state = em.getComponent<stateComponent>(entity);

        if (!pos || !size) continue;

        //Skips nodes outside view
        if (pos->x + size->width < left || pos->x > right || 
            pos->y + size->height < top || pos->y > bottom) 
            continue;

        Vec2 screen = worldToScreen(canvas, pos->x, pos->y);

        SDL_FRect rect;
        rect.x = screen.x;
        rect.y = screen.y;
        rect.w = size->width * canvas.zoom;
        rect.h = size->height * canvas.zoom;

        //Base colour by node type
        auto* style = em.getComponent<StyleComponent>(entity);

        if (style) {
            SDL_SetRenderDrawColor(
                renderer,
                style->backgroundColour.r,
                style->backgroundColour.g,
                style->backgroundColour.b,
                style->backgroundColour.a
            );
        }

        //Override for hovered or selected state
        if (state && state->isSelected) {
            SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
        }
        else if (state && state->isHovered) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 150, 255);
        }

        SDL_RenderFillRect(renderer, &rect);

        auto* text = em.getComponent<TextComponent>(entity);
        if (text && !text->text.empty() && font) {

            SDL_Color color = {0, 0, 0, 255};
            int wrapWidth = std::max(10, (int)(size->width - 10));

            SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(
                font,
                text->text.c_str(),
                text->text.length(),
                color,
                wrapWidth
            );

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

            float scale = canvas.zoom;

            SDL_FRect textRect;
            textRect.x = rect.x + 5;
            textRect.y = rect.y + 5;
            textRect.w = surface->w * scale;
            textRect.h = surface->h * scale;

            SDL_RenderTexture(renderer, texture, nullptr, &textRect);

            SDL_DestroyTexture(texture);
            SDL_DestroySurface(surface);
        }
    }
}

//Renders the UI bar and tool buttons
void renderUI(SDL_Renderer* renderer, const UI& ui, const InputState& input, const Canvas& canvas) {
    float dragSize = 25.0f;

    //UI body
    SDL_FRect body;
    bool showFull = ui.isPinned || ui.isHovered;
    if (!showFull) {
        if (ui.orientation == Orientation::Vertical)
            body = { ui.uiX, ui.uiY, ui.uiW, dragSize };
        else
            body = { ui.uiX, ui.uiY, dragSize, ui.uiH };
    } else {
        body = { ui.uiX, ui.uiY, ui.uiW, ui.uiH };
    }

    SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
    SDL_RenderFillRect(renderer, &body);

    //Drag handle
    SDL_FRect dragZone;
    bool hovering = false;
    if (ui.orientation == Orientation::Vertical) {
        dragZone = { ui.uiX, ui.uiY, ui.uiW, dragSize };
        hovering = input.mouseX >= dragZone.x && input.mouseX <= dragZone.x + dragZone.w &&
                   input.mouseY >= dragZone.y && input.mouseY <= dragZone.y + dragZone.h;
    } else {
        dragZone = { ui.uiX, ui.uiY, dragSize, ui.uiH };
        hovering = input.mouseX >= dragZone.x && input.mouseX <= dragZone.x + dragZone.w &&
                   input.mouseY >= dragZone.y && input.mouseY <= dragZone.y + dragZone.h;
    }
    SDL_SetRenderDrawColor(renderer,
        hovering ? 90 : 70,
        hovering ? 90 : 70,
        hovering ? 90 : 70,
        255
    );
    SDL_RenderFillRect(renderer, &dragZone);

    //render tool buttons if UI fully visible
    if (showFull) {
        for (const auto& btn : ui.toolButtons) {
            SDL_FRect rect = { btn.x, btn.y, btn.w, btn.h };
            SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
            SDL_RenderFillRect(renderer, &rect);

            bool over = input.mouseX >= rect.x && input.mouseX <= rect.x + rect.w &&
                        input.mouseY >= rect.y && input.mouseY <= rect.y + rect.h;
            if (over) {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                SDL_RenderRect(renderer, &rect);
            }
        }
    }

    //Render ghost node outline when dragging tool
    if (ui.isDraggingTool) {
        Vec2 size;
        if (ui.draggingToolType == NodeType::Line) {
            size.x = 200.0f;
            size.y = 15.0f;
        } else {
            size = getNodeBaseSize(ui.draggingToolType);
        }

        SDL_FRect ghost;
        ghost.w = std::max(size.x * canvas.zoom, 1.0f);
        ghost.h = std::max(size.y * canvas.zoom, 1.0f);
        ghost.x = ui.toolDragX - ghost.w / 2.0f;
        ghost.y = ui.toolDragY - ghost.h / 2.0f;

        switch (ui.draggingToolType) {
            case NodeType::Note: SDL_SetRenderDrawColor(renderer, 255, 255, 100, 200); break;
            case NodeType::Text: SDL_SetRenderDrawColor(renderer, 100, 200, 255, 200); break;
            case NodeType::Image: SDL_SetRenderDrawColor(renderer, 255, 150, 150, 200); break;
            case NodeType::ToDo: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200); break;
            case NodeType::Link: SDL_SetRenderDrawColor(renderer, 245, 222, 179, 200); break;
            case NodeType::Grid: SDL_SetRenderDrawColor(renderer, 152, 251, 152, 200); break;
            case NodeType::Line: SDL_SetRenderDrawColor(renderer, 30, 144, 255, 200); break;
            //case NodeType::Draw: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200); break;
            case NodeType::Colour: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200); break;
            case NodeType::Comment: SDL_SetRenderDrawColor(renderer, 255, 228, 225, 200); break;
            case NodeType::Code: SDL_SetRenderDrawColor(renderer, 112, 128, 144, 200); break;
            default: SDL_SetRenderDrawColor(renderer, 200, 200, 200, 200); break;
        }

        SDL_RenderFillRect(renderer, &ghost);
    }
}

//Renders the side panels
void renderPanels(SDL_Renderer* renderer, TTF_Font* font, EntityManager& em, Panels& panels) {
    panels.entries.clear();

    SDL_FRect top = { panels.top.x, panels.top.y, panels.top.w, panels.top.h };
    SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
    SDL_RenderFillRect(renderer, &top);

    SDL_FRect bottom = { panels.bottom.x, panels.bottom.y, panels.bottom.w, panels.bottom.h };
    SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
    SDL_RenderFillRect(renderer, &bottom);

    SDL_FRect divider = { panels.top.x, panels.top.y + panels.top.h, panels.top.w, panels.dividerHeight };
    SDL_SetRenderDrawColor(renderer, 110, 110,110, 255);
    SDL_RenderFillRect(renderer, &divider);

    if (!font) return;

    float padding = 5.0f;
    float yOffset = panels.top.y + padding;

    //Makes new list to display
    std::vector<Entity> entities = em.getEntities();
    std::sort(entities.begin(), entities.end(), [](const Entity& a, const Entity& b) {
        return a.id < b.id;
    });

    for (auto& e : entities) {
        auto* type = em.getComponent<NodeTypeComponent>(e);
        if (!type) continue;

        std::string label = std::string(nodeTypeToString(type->type)) + " [" + std::to_string(e.id) + "]";

        SDL_Color color = { 255, 255, 255, 255 };

        SDL_Surface* surface = TTF_RenderText_Blended(
            font,
            label.c_str(),
            label.length(),
            color
        );

        if (!surface) continue;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_FRect textRect;
        textRect.x = panels.top.x + padding;
        textRect.y = yOffset;
        textRect.w = (float)surface->w;
        textRect.h = (float)surface->h;

        panels.entries.push_back({
            e,
            textRect.x,
            textRect.y,
            textRect.w,
            textRect.h
        });

        SDL_RenderTexture(renderer, texture, nullptr, &textRect);

        yOffset += surface->h + 4;

        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);

        if (yOffset > panels.top.y + panels.top.h) break;
    }
}

//Presents the rendered frame to screen
void endFrame(SDL_Renderer* renderer) {
    SDL_RenderPresent(renderer);
}