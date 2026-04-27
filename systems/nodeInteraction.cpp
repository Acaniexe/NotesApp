#include "nodeInteraction.h"

void updateNodeInteraction(InputState& input, EntityManager& em, const Canvas& canvas, const UI& ui) {
    if (isMouseOverUI(ui, input.mouseX, input.mouseY)) return;

    Vec2 mouseWorld = screenToWorld(canvas, input.mouseX, input.mouseY);
    static bool draggingStarted = false;
    Entity* clicked = nullptr;

    for (auto& entity : em.getEntities()) {
        auto* pos = em.getComponent<PositionComponent>(entity);
        auto* size = em.getComponent<sizeComponent>(entity);
        auto* state = em.getComponent<stateComponent>(entity);
        if (!pos || !size || !state) continue;

        state->isHovered = false;
        bool hovered = mouseWorld.x >= pos->x && mouseWorld.x <= pos->x + size->width &&
                       mouseWorld.y >= pos->y && mouseWorld.y <= pos->y + size->height;
        state->isHovered = hovered;

        if (hovered) clicked = &entity;
    }

    if (input.delPressed) {
        deleteNode(em);
    }

    if (input.leftDown) {
        if (clicked) {
            auto* clickedState = em.getComponent<stateComponent>(*clicked);
            if (!clickedState) return;

            bool multi = input.ctrlHeld;

            if (!multi) {
                for (auto& e : em.getEntities()) {
                    auto* st = em.getComponent<stateComponent>(e);
                    if (!st) continue;
                    if (&e != clicked) st->isSelected = false;
                }
            }

            clickedState->isSelected = true;
        } else {
            for (auto& e : em.getEntities()) {
                auto* st = em.getComponent<stateComponent>(e);
                if (!st) continue;
                st->isSelected = false;
            }
        }

        draggingStarted = false;
    }

    if (input.rightDown && clicked) {
        auto* clickedState = em.getComponent<stateComponent>(*clicked);
        if (!clickedState) return;

        clickedState->isSelected = false;
    }

    if (input.leftHeld && !ui.isDraggingTool) {
        for (auto& e : em.getEntities()) {
            auto* pos = em.getComponent<PositionComponent>(e);
            auto* st = em.getComponent<stateComponent>(e);
            if (!pos || !st || !st->isSelected) continue;

            if (!draggingStarted) {
                st->isDragging = true;
                st->dragOffset = { pos->x - mouseWorld.x, pos->y - mouseWorld.y };
            }

            if (st->isDragging) {
                pos->x = mouseWorld.x + st->dragOffset.x;
                pos->y = mouseWorld.y + st->dragOffset.y;
            }
        }
        draggingStarted = true;
    }

    if (input.leftReleased && !ui.isDraggingTool) {
        for (auto& e : em.getEntities()) {
            auto* st = em.getComponent<stateComponent>(e);
            if (!st) continue;
            st->isDragging = false;
        }
        draggingStarted = false;
    }
}