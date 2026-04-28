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

    if (ui.orientation == Orientation::Horizontal) {
        ui.uiW = length;
        ui.uiH = thickness;
    } else {
        ui.uiW = thickness;
        ui.uiH = length;
    }

    float x = 0.0f;
    float y = 0.0f;

    switch (ui.dock) {
        case DockSide::Top:    y = 0.0f; break;
        case DockSide::Bottom: y = ui.baseH - ui.uiH; break;
        case DockSide::Left:   x = 0.0f; break;
        case DockSide::Right:  x = ui.baseW - ui.uiW; break;
    }

    if (!ui.isPinned && !ui.isHovered) {
        switch (ui.dock) {
            case DockSide::Top:    y = -ui.uiH; break;
            case DockSide::Bottom: y = ui.baseH; break;
            case DockSide::Left:   x = -ui.uiW; break;
            case DockSide::Right:  x = ui.baseW; break;
        }
    }

    ui.uiX = x;
    ui.uiY = y;
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

    if (!ui.isPinned) {
        bool edgeHover = false;
        switch (ui.dock) {
            case DockSide::Top:    edgeHover = input.mouseY <= 5.0f; break;
            case DockSide::Bottom: edgeHover = input.mouseY >= ui.baseH - 5.0f; break;
            case DockSide::Left:   edgeHover = input.mouseX <= 5.0f; break;
            case DockSide::Right:  edgeHover = input.mouseX >= ui.baseW - 5.0f; break;
        }

        bool overUI = input.mouseX >= ui.uiX && input.mouseX <= ui.uiX + ui.uiW &&
                      input.mouseY >= ui.uiY && input.mouseY <= ui.uiY + ui.uiH;

        ui.isHovered = edgeHover || overUI;
    } else {
        ui.isHovered = false;
    }

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
            ui.isPinned = !ui.isPinned;
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
        ui.isPinned = !ui.isPinned;
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
            Vec2 size = getNodeBaseSize(ui.draggingToolType);
            world.x -= size.x * 0.5f;
            world.y -= size.y * 0.5f;
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

void updatePanels(Panels& panels, int windowWidth, int windowHeight) {
    float baseW = (float)windowWidth;
    float baseH = (float)windowHeight;

    float widthPercentage = 0.15f;
    float panelWidth = std::clamp(baseW * widthPercentage, 125.0f, 250.0f);
    
    float divider = panels.dividerHeight;

    float usableHeight = baseH - divider;
    float topHeight = usableHeight * panels.split;
    float bottomHeight = usableHeight * (1.0f - panels.split);
    float x = baseW - panelWidth;

    panels.top.x = x;
    panels.top.y = 0.0f;
    panels.top.w = panelWidth;
    panels.top.h = topHeight;

    panels.bottom.x = x;
    panels.bottom.y = topHeight + divider;
    panels.bottom.w = panelWidth;
    panels.bottom.h = bottomHeight;
}

void updatePanelsState(Panels& panels, InputState& input, int windowWidth, int windowHeight) {
    float baseW = (float)windowWidth;

    float panelWidth = std::clamp(baseW * 0.15f, 125.0f, 250.0f);
    float x = baseW - panelWidth;

    float dividerY = panels.top.y + panels.top.h;

    bool overDivider = 
        input.mouseX >= x &&
        input.mouseX <= x + panelWidth &&
        input.mouseY >= dividerY &&
        input.mouseY <= dividerY + panels.dividerHeight;

    if (input.leftDown && overDivider) {
        panels.isDraggingDivider = true;
    }

    if (panels.isDraggingDivider && input.leftHeld) {
        float mouseY = input.mouseY;

        float newSplit = mouseY / (float)windowHeight;
        panels.split = std::clamp(newSplit, 0.1f, 0.9f);
    }

    if (input.leftReleased) {
        panels.isDraggingDivider = false;
    }
}