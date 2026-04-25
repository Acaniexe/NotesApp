#include "ui.h"
#include <algorithm>

void updateUILayout(UI& ui, int windowWidth, int windowHeight) {
    ui.baseW = (float)windowWidth;
    ui.baseH = (float)windowHeight;

    if (ui.isDragging) return;

    ui.orientation = (ui.dock == DockSide::Top || ui.dock == DockSide::Bottom) ? Orientation::Horizontal : Orientation::Vertical;

    float length = (ui.orientation == Orientation::Horizontal) ? ui.baseW : ui.baseH;

    float thicknessPercent = (ui.orientation == Orientation::Horizontal) ? 0.12f : 0.08f; 
    float thickness = (ui.orientation == Orientation::Horizontal) ? ui.baseH * thicknessPercent : ui.baseW * thicknessPercent;
    thickness = std::clamp(thickness, 60.0f, 100.0f);

    if (ui.dock == DockSide::Top || ui.dock == DockSide::Bottom) {
        ui.uiW = length;
        ui.uiH = thickness;
    } else {
        ui.uiW = thickness;
        ui.uiH = length;
    }

    switch (ui.dock) {
        case DockSide::Top:    ui.uiX = 0.0f; ui.uiY = 0.0f; break;
        case DockSide::Bottom: ui.uiX = 0.0f; ui.uiY = ui.baseH - ui.uiH; break;
        case DockSide::Left:   ui.uiX = 0.0f; ui.uiY = 0.0f; break;
        case DockSide::Right:  ui.uiX = ui.baseW - ui.uiW; ui.uiY = 0.0f; break;
    }
}

void updateToolButtons(UI& ui) {
    float padding = 10.0f;
    float buttonSize = 40.0f;
    float offset = padding;

    ui.toolButtons.clear();
    NodeType types[] = { 
        NodeType::Note, NodeType::Text, NodeType::Image, NodeType::ToDo, 
        NodeType::Link, NodeType::Grid, NodeType::Line, NodeType::Draw,
        NodeType::Colour, NodeType::Comment, NodeType::Code
    };

    float buttonOffset = 25.0f + 10.0f;

    for (NodeType t : types) {
        if (ui.orientation == Orientation::Vertical) {
            ui.toolButtons.push_back({ t, ui.uiX + padding, ui.uiY + offset + buttonOffset, ui.uiW - 2*padding, buttonSize });
            offset += buttonSize + padding;
        } else {
            ui.toolButtons.push_back({ t, ui.uiX + offset + buttonOffset, ui.uiY + padding, buttonSize, ui.uiH - 2*padding });
            offset += buttonSize + padding;
        }
    }
}

void updateUIState(InputState& input, UI& ui, Canvas& canvas, EntityManager& entityManager) {
    float dragSize = 25.0f;
    bool mouseOverDragZone = (ui.orientation == Orientation::Vertical) ?
        (input.mouseX >= ui.uiX && input.mouseX <= ui.uiX + ui.uiW &&
         input.mouseY >= ui.uiY && input.mouseY <= ui.uiY + dragSize) :
        (input.mouseX >= ui.uiX && input.mouseX <= ui.uiX + dragSize &&
         input.mouseY >= ui.uiY && input.mouseY <= ui.uiY + ui.uiH);

    float dragThreshold = 5.0f;

    if (input.leftDown && mouseOverDragZone) {
        ui.isDragging = true;
        ui.dragStarted = false;
        ui.dragOffsetX = input.mouseX - ui.uiX;
        ui.dragOffsetY = input.mouseY - ui.uiY;
        ui.clickStartX = input.mouseX;
        ui.clickStartY = input.mouseY;
    }

    if (ui.isDragging && input.leftHeld) {
        float dx = input.mouseX - ui.clickStartX;
        float dy = input.mouseY - ui.clickStartY;
        if (!ui.dragStarted && dx*dx + dy*dy > dragThreshold*dragThreshold) {
            ui.dragStarted = true;
        }
        if (ui.dragStarted) {
            ui.uiX = input.mouseX - ui.dragOffsetX;
            ui.uiY = input.mouseY - ui.dragOffsetY;
        }
    }

    if (ui.isDragging && input.leftReleased) {
        ui.isDragging = false;
        if (!ui.dragStarted && mouseOverDragZone) {
            ui.isCollapsed = !ui.isCollapsed;
        }

        float margin = 100.0f;
        if (ui.dragStarted) {
            if (input.mouseY < margin) ui.dock = DockSide::Top;
            else if (input.mouseY > ui.baseH - margin) ui.dock = DockSide::Bottom;
            else if (input.mouseX < margin) ui.dock = DockSide::Left;
            else if (input.mouseX > ui.baseW - margin) ui.dock = DockSide::Right;
        }
    }
    
    if (input.dockCollapsePressed) {
        ui.isCollapsed = !ui.isCollapsed;
        input.dockCollapsePressed = false;
    }
    
    for (auto& btn : ui.toolButtons) {
        bool over = input.mouseX >= btn.x && input.mouseX <= btn.x + btn.w &&
                    input.mouseY >= btn.y && input.mouseY <= btn.y + btn.h;
        if (input.leftDown && over) {
            ui.isDraggingTool = true;
            ui.draggingToolType = btn.type;
            ui.toolDragX = input.mouseX;
            ui.toolDragY = input.mouseY;
            break;
        }
    }

    if (ui.isDraggingTool && input.leftHeld) {
        ui.toolDragX = input.mouseX;
        ui.toolDragY = input.mouseY;
    }

    if (ui.isDraggingTool && input.leftReleased) {
        if (!isMouseOverUI(ui, input.mouseX, input.mouseY)) {
            Vec2 world = screenToWorld(canvas, input.mouseX, input.mouseY);
            createNode(entityManager, ui.draggingToolType, world.x, world.y);
        }
        ui.isDraggingTool = false;
    }
}

bool isMouseOverUI(const UI& ui, float mx, float my) {
    return mx >= ui.uiX &&
           mx <= ui.uiX + ui.uiW &&
           my >= ui.uiY &&
           my <= ui.uiY + ui.uiH;
}