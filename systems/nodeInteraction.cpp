#include "nodeInteraction.h"
#include <iostream>

bool isInResizeZone(float mx, float my, PositionComponent* pos, sizeComponent* size) {
    const float handleSize = 10.0f;

    float right = pos->x + size->width;
    float bottom = pos->y + size->height;

    return (mx >= right - handleSize && mx <= right &&
            my >= bottom - handleSize && my <= bottom);
}

void updateNodeInteraction(InputState& input, EntityManager& em, Canvas& canvas, const UI& ui, Panels& panels, int windowWidth, int windowHeight) {
    //Ignore node interaction if mouse over UI
    if (isMouseOverUI(ui, input.mouseX, input.mouseY)) return;

    //Node deletion
    if (input.delPressed && !input.isTyping) {
        deleteNode(em);
        input.delPressed = false;
    }

    bool overTopPanel = 
        input.mouseX >= panels.top.x &&
        input.mouseX <= panels.top.x + panels.top.w &&
        input.mouseY >= panels.top.y &&
        input.mouseY <= panels.top.y + panels.top.h;

    bool overBottomPanel = 
        input.mouseX >= panels.bottom.x &&
        input.mouseX <= panels.bottom.x + panels.bottom.w &&
        input.mouseY >= panels.bottom.y &&
        input.mouseY <= panels.bottom.y + panels.bottom.h;

    if (overTopPanel || overBottomPanel) return;

    Vec2 mouseWorld = screenToWorld(canvas, input.mouseX, input.mouseY);
    static bool draggingStarted = false;
    Entity* clicked = nullptr;
    int windowX = windowWidth / 2.0f;
    int windowY = windowHeight / 2.0f;

    //Hover detection
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

    //Selection handling 
    if (input.leftDown) {
        if (clicked) {
            auto* pos = em.getComponent<PositionComponent>(*clicked);
            auto* size = em.getComponent<sizeComponent>(*clicked);
            auto* st = em.getComponent<stateComponent>(*clicked);
            if (!pos || !size || !st) return;

            bool multi = input.ctrlHeld;

            //single selection
            if (!multi) {
                for (auto& e : em.getEntities()) {
                    auto* st = em.getComponent<stateComponent>(e);
                    if (!st) continue;
                    if (&e != clicked) {
                        st->isSelected = false; 
                        st->isEditing = false;
                    }
                }
            }

            st->isSelected = true;

            em.bringToFront(*clicked);

            if (!isInResizeZone(mouseWorld.x, mouseWorld.y, pos, size)) {
                for (auto& e : em.getEntities()) {
                    auto* p = em.getComponent<PositionComponent>(e);
                    auto* s = em.getComponent<stateComponent>(e);
                    if (!p || !s || !s->isSelected) continue;

                    s->isDragging = true;
                    s->dragOffset = { p->x - mouseWorld.x, p->y - mouseWorld.y };
                }
            } else {
                st->isResizing = true;
            }
        } else {
            //Click empty space -> clear selection
            for (auto& e : em.getEntities()) {
                auto* st = em.getComponent<stateComponent>(e);
                if (!st) continue;
                st->isSelected = false;
                st->isEditing = false;
            }
        }

        draggingStarted = false;
    }

    //Deselect Node
    if (input.rightDown && clicked) {
        auto* clickedState = em.getComponent<stateComponent>(*clicked);
        if (!clickedState) return;

        clickedState->isSelected = false;
    }

    //Drag selected node/s
    if (input.leftHeld && !ui.isDraggingTool) {
        for (auto& e : em.getEntities()) {
            auto* pos = em.getComponent<PositionComponent>(e);
            auto* size = em.getComponent<sizeComponent>(e);
            auto* st = em.getComponent<stateComponent>(e);

            if (!pos || !size || !st || !st->isSelected) continue;

            if (st->isResizing) {
                size->width = std::max(50.0f, mouseWorld.x - pos->x);
                size->height = std::max(30.0f, mouseWorld.y - pos->y);
            }
            else if (st->isDragging) {
                //Apply drag movement
                pos->x = mouseWorld.x + st->dragOffset.x;
                pos->y = mouseWorld.y + st->dragOffset.y;
            } 
        }
        draggingStarted = true;
    }

    //End drag
    if (input.leftReleased && !ui.isDraggingTool) {
        for (auto& e : em.getEntities()) {
            auto* st = em.getComponent<stateComponent>(e);
            if (!st) continue;
            st->isDragging = false;
            st->isResizing = false;
        }
        draggingStarted = false;
    }

    if (input.ctrlHeld && input.findNode) {
        for (auto& e : em.getEntities()) {
            auto* pos = em.getComponent<PositionComponent>(e);
            auto* st = em.getComponent<stateComponent>(e);
            auto* size = em.getComponent<sizeComponent>(e);
            if (!pos || !st || !size || !st->isSelected) continue;
        
            canvas.cameraX = (pos->x + size->width * 0.5f) - (windowX / canvas.zoom);
            canvas.cameraY = (pos->y + size->height * 0.5f) - (windowY / canvas.zoom);
        }
    }

    if (input.ctrlHeld && input.duplicateNode) {
        Vec2 world = screenToWorld(canvas, input.mouseX, input.mouseY);

        std::vector<Entity> selected;

        for (auto& e : em.getEntities()) {
            auto* st = em.getComponent<stateComponent>(e);

            if (st && st->isSelected)
                selected.push_back(e);
        }

        for (auto& e : selected) {
            cloneNode(em, e, world.x + 20.0f, world.y + 20.0f);
        }

        input.duplicateNode = false;
    }
}