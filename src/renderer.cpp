#include "renderer.h"

// logic to render background
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
            case NodeType::Note:
                SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
                break;

            case NodeType::Text:
                SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
                break;

            case NodeType::Image:
                SDL_SetRenderDrawColor(renderer, 255, 150, 150, 255);
                break;

            case NodeType::ToDo:
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                break;
            default:
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                break;
        }

        SDL_RenderFillRect(renderer, &rect);
    }
}

void renderUI(SDL_Renderer* renderer, const UI& ui, const InputState& input) {
    SDL_FRect body = {ui.uiX, ui.uiY, ui.uiW, ui.uiH};
    
    if (!ui.isCollapsed) {
        SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
        SDL_RenderFillRect(renderer, &body);
    }

   float dragSize = 25.0f;

    bool hovering = false;

    if (ui.orientation == Orientation::Vertical) {
        hovering = 
            input.mouseX >= ui.uiX &&
            input.mouseX <= ui.uiX + ui.uiW &&
            input.mouseY >= ui.uiY &&
            input.mouseY <= ui.uiY + dragSize;

        SDL_SetRenderDrawColor(renderer,
            hovering ? 90 : 70,
            hovering ? 90 : 70,
            hovering ? 90 : 70,
            255
        );

        SDL_FRect dragZone = {ui.uiX, ui.uiY, ui.uiW, dragSize};
        SDL_RenderFillRect(renderer, &dragZone);
    } else {
        hovering =
            input.mouseX >= ui.uiX &&
            input.mouseX <= ui.uiX + dragSize &&
            input.mouseY >= ui.uiY &&
            input.mouseY <= ui.uiY + ui.uiH;

        SDL_SetRenderDrawColor(renderer,
            hovering ? 90 : 70,
            hovering ? 90 : 70,
            hovering ? 90 : 70,
            255
        );

        SDL_FRect dragZone = {ui.uiX, ui.uiY, dragSize, ui.uiH};
        SDL_RenderFillRect(renderer, &dragZone);
    }
};

void endFrame(SDL_Renderer* renderer) {
    SDL_RenderPresent(renderer);
}