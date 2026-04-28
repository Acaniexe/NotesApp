#include "renderer.h"

void beginFrame(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 230, 225, 215, 255);
    SDL_RenderClear(renderer);
}

void renderCanvas(SDL_Renderer* renderer, const Canvas& canvas, int windowWidth, int windowHeight) {

    float alpha = SDL_clamp(canvas.zoom * 0.5f, 0.15f, 0.5f);
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, (Uint8)(alpha * 255));

    float spacing = 100.0f;

    float left = canvas.cameraX;
    float right = canvas.cameraX + (windowWidth / canvas.zoom);
    float top = canvas.cameraY;
    float bottom = canvas.cameraY + (windowHeight / canvas.zoom);

    float startX = floorf(left / spacing) * spacing;
    float startY = floorf(top / spacing) * spacing;

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

void renderNodes(SDL_Renderer* renderer, EntityManager& em, const Canvas& canvas, float windowWidth, float windowHeight) {
    float margin = 200.0f;

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

        float nodeRight = pos->x + size->width;
        float nodeBottom = pos->y + size->height;

        if (nodeRight < left || pos->x > right || nodeBottom < top || pos->y > bottom) continue;

        Vec2 screen = worldToScreen(canvas, pos->x, pos->y);

        SDL_FRect rect;
        rect.x = screen.x;
        rect.y = screen.y;
        rect.w = size->width * canvas.zoom;
        rect.h = size->height * canvas.zoom;

        switch (type ? type->type : NodeType::Note) {
            case NodeType::Note: SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255); break;
            case NodeType::Text: SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255); break;
            case NodeType::Image: SDL_SetRenderDrawColor(renderer, 255, 150, 150, 255); break;
            case NodeType::ToDo: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); break;
            case NodeType::Link: SDL_SetRenderDrawColor(renderer, 245, 222, 179, 255); break;
            case NodeType::Grid: SDL_SetRenderDrawColor(renderer, 152, 251, 152, 255); break;
            case NodeType::Line: SDL_SetRenderDrawColor(renderer, 30, 144, 255, 255); break;
            case NodeType::Draw: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200); break;
            case NodeType::Colour: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); break;
            case NodeType::Comment: SDL_SetRenderDrawColor(renderer, 255, 228, 225, 255); break;
            case NodeType::Code: SDL_SetRenderDrawColor(renderer, 112, 128, 144, 255); break;
            default: SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); break;
        }

        if (state && state->isSelected) {
            SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
        }
        else if (state && state->isHovered) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 150, 255);
        }

        SDL_RenderFillRect(renderer, &rect);
    }
}

void renderUI(SDL_Renderer* renderer, const UI& ui, const InputState& input, const Canvas& canvas) {
    float dragSize = 25.0f;

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
            case NodeType::Draw: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200); break;
            case NodeType::Colour: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200); break;
            case NodeType::Comment: SDL_SetRenderDrawColor(renderer, 255, 228, 225, 200); break;
            case NodeType::Code: SDL_SetRenderDrawColor(renderer, 112, 128, 144, 200); break;
            default: SDL_SetRenderDrawColor(renderer, 200, 200, 200, 200); break;
        }

        SDL_RenderFillRect(renderer, &ghost);
    }
}

void renderPanels(SDL_Renderer* renderer, const Panels& panels) {
    SDL_FRect top = { panels.top.x, panels.top.y, panels.top.w, panels.top.h };
    SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
    SDL_RenderFillRect(renderer, &top);

    SDL_FRect bottom = { panels.bottom.x, panels.bottom.y, panels.bottom.w, panels.bottom.h };
    SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
    SDL_RenderFillRect(renderer, &bottom);

    SDL_FRect divider = { panels.top.x, panels.top.y + panels.top.h, panels.top.w, panels.dividerHeight };
    SDL_SetRenderDrawColor(renderer, 110, 110,110, 255);
    SDL_RenderFillRect(renderer, &divider);
}

void endFrame(SDL_Renderer* renderer) {
    SDL_RenderPresent(renderer);
}