#include "ui.h"
#include <algorithm>

//Updates UI Layout based on window size, dock position and panels
void updateUILayout(UI& ui, int windowWidth, int windowHeight, const Panels& panels) {
    ui.baseW = (float)windowWidth;
    ui.baseH = (float)windowHeight;
    if (ui.isDragging) return; //Skips layout update while dragging

    //determines orientation based on dock side
    ui.orientation = (ui.dock == DockSide::Top || ui.dock == DockSide::Bottom) ? Orientation::Horizontal : Orientation::Vertical;

    float length = (ui.orientation == Orientation::Horizontal) ? ui.baseW : ui.baseH;

    //Sets initial thickness
    if (ui.uiThickness <= 0.0f) {
        float thicknessPercent = (ui.orientation == Orientation::Horizontal) ? 0.12f : 0.08f;
        ui.uiThickness = (ui.orientation == Orientation::Horizontal) ? ui.baseH * thicknessPercent : ui.baseW * thicknessPercent;
        ui.uiThickness = std::clamp(ui.uiThickness, ui.minThickness, ui.maxThickness);
    }

    //Assigns width and height based on orientation
    if (ui.orientation == Orientation::Horizontal) {
        ui.uiW = length;
        ui.uiH = ui.uiThickness;
    } else {
        ui.uiW = ui.uiThickness;
        ui.uiH = length;
    }

    float x = 0.0f;
    float y = 0.0f;

    float panelOffset = (ui.dock == DockSide::Right) ? panels.panelWidth : 0.0f;

    //Position UI based on dock side
    switch (ui.dock) {
        case DockSide::Top:    y = 0.0f; break;
        case DockSide::Bottom: y = ui.baseH - ui.uiH; break;
        case DockSide::Left:   x = 0.0f; break;
        case DockSide::Right:  x = ui.baseW - ui.uiW - panelOffset; break;
    }

    //Retracts UI if unpinned and not hovered
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

//Updates positions and sized of tool buttons
void updateToolButtons(UI& ui) {
    float padding = 10.0f;
    float buttonSize = 40.0f;
    float offset = padding;

    ui.toolButtons.clear();

    NodeType types[] = { NodeType::Note, NodeType::Text, NodeType::Image, NodeType::ToDo, NodeType::Link,
                         NodeType::Grid, NodeType::Line, NodeType::Colour, NodeType::Comment, NodeType::Code };

    float buttonOffset = 25.0f + 10.0f; //Visual spacing offset

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

//Handles resizing of the toolbar via dragging edge
void updateToolbarResize(UI& ui, InputState& input, const Panels& panels) {
    float handleSize = 6.0f;

    float panelOffset = (ui.dock == DockSide::Right) ? panels.panelWidth : 0.0f;

    bool overHandle = false;

    //Determines which edge is draggable depending on orientation 
    if (ui.orientation == Orientation::Horizontal) {
        overHandle = input.mouseY >= ui.uiY + ui.uiH - handleSize &&
                     input.mouseY <= ui.uiY + ui.uiH + handleSize &&
                     input.mouseX >= ui.uiX &&
                     input.mouseX <= ui.uiX + ui.uiW;
    } else {
        if (ui.dock == DockSide::Left) {
            overHandle = input.mouseX >= ui.uiX + ui.uiW - handleSize &&
                         input.mouseX <= ui.uiX + ui.uiW + handleSize &&
                         input.mouseY >= ui.uiY &&
                         input.mouseY <= ui.uiY + ui.uiH;
        } else if (ui.dock == DockSide::Right) {
            overHandle = input.mouseX >= ui.uiX - handleSize &&
                         input.mouseX <= ui.uiX + handleSize &&
                         input.mouseY >= ui.uiY &&
                         input.mouseY <= ui.uiY + ui.uiH;
        }
    }

    if (input.leftDown && overHandle) {
        ui.isDraggingThickness = true;
    }

    //Updates thickness based on drag and clamps 
    if (ui.isDraggingThickness && input.leftHeld) {
        if (ui.orientation == Orientation::Horizontal) {
            ui.uiThickness = std::clamp(input.mouseY - ui.uiY, ui.minThickness, ui.maxThickness);
        } else {
            if (ui.dock == DockSide::Left) {
                ui.uiThickness = std::clamp(input.mouseX - ui.uiX, ui.minThickness, ui.maxThickness);
            } else if (ui.dock == DockSide::Right) {
                ui.uiThickness = std::clamp(ui.baseW - input.mouseX - panelOffset, ui.minThickness, ui.maxThickness);
            }
        }
    }

    if (input.leftReleased) {
        ui.isDraggingThickness = false;
    }
}

//Updates UI state: dragging, hovering, tool interactions, pin/unpin, docking
void updateUIState(InputState& input, UI& ui, Canvas& canvas, EntityManager& entityManager, const Panels& panels) {
    float dragSize = 25.0f;
    bool mouseOverDragZone = (ui.orientation == Orientation::Vertical) ? 
        (input.mouseX >= ui.uiX && input.mouseX <= ui.uiX + ui.uiW &&
         input.mouseY >= ui.uiY && input.mouseY <= ui.uiY + dragSize) :
        (input.mouseX >= ui.uiX && input.mouseX <= ui.uiX + dragSize &&
         input.mouseY >= ui.uiY && input.mouseY <= ui.uiY + ui.uiH);

    float dragThreshold = 5.0f;

    //Updates hover state for auto-hide UI
    if (!ui.isPinned) {
        bool edgeHover = false;

        float panelOffset = (ui.dock == DockSide::Right) ? panels.panelWidth : 0.0f;

        switch (ui.dock) {
            case DockSide::Top:    
                edgeHover = input.mouseY <= 5.0f; 
                break;
            case DockSide::Bottom: 
                edgeHover = input.mouseY >= ui.baseH - 5.0f; 
                break;
            case DockSide::Left:   
                edgeHover = input.mouseX <= 5.0f; 
                break;
            case DockSide::Right:
                edgeHover = input.mouseX >= ui.baseW - ui.uiW - panelOffset &&
                            input.mouseX <= ui.baseW - panelOffset;
                break;
        }
        bool overUI = input.mouseX >= ui.uiX && input.mouseX <= ui.uiX + ui.uiW &&
                      input.mouseY >= ui.uiY && input.mouseY <= ui.uiY + ui.uiH;
        ui.isHovered = edgeHover || overUI;
    } else {
        ui.isHovered = false;
    }

    //Starts dragging UI
    if (input.leftDown && mouseOverDragZone) {
        ui.isDragging = true;
        ui.dragStarted = false;
        ui.dragOffsetX = input.mouseX - ui.uiX;
        ui.dragOffsetY = input.mouseY - ui.uiY;
        ui.clickStartX = input.mouseX;
        ui.clickStartY = input.mouseY;
    }

    //Handles dragging movement
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

    //Handles drag release: pin/unpin or dock
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
            else if (input.mouseX > ui.baseW - panels.panelWidth - (ui.uiW * 0.5f))
                ui.dock = DockSide::Right;
        }
    }

    //Handles pin state
    if (input.dockCollapsePressed) {
        ui.isPinned = !ui.isPinned;
        input.dockCollapsePressed = false;
    }

    //Handles tool button dragging
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

    //Places tool in canvas if released outside UI
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
//Return true if mouse is over the UI
bool isMouseOverUI(const UI& ui, float mx, float my) {
    return mx >= ui.uiX && mx <= ui.uiX + ui.uiW &&
           my >= ui.uiY && my <= ui.uiY + ui.uiH;
}

//Updates panel sizes and positions based on window 
void updatePanels(Panels& panels, int windowWidth, int windowHeight) {
    float baseW = (float)windowWidth;
    float baseH = (float)windowHeight;

    float panelWidth = panels.panelWidth;
    if (panelWidth <= 0.0f) {
        panelWidth = std::clamp(baseW * 0.15f, panels.minWidth, panels.maxWidth);
        panels.panelWidth = panelWidth;
    }

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

//Handles panel state: dragging divider and resize panel width
void updatePanelsState(Panels& panels, EntityManager& em, InputState& input, int windowWidth, int windowHeight) {
    float baseW = (float)windowWidth;
    float panelX = baseW - panels.panelWidth;
    float dividerY = panels.top.y + panels.top.h;

    float handleSize = 6.0f;

    //Dragging vertical divider
    bool overDivider = input.mouseX >= panelX && input.mouseX <= panelX + panels.panelWidth &&
                       input.mouseY >= dividerY && input.mouseY <= dividerY + panels.dividerHeight;
    if (input.leftDown && overDivider) {
        panels.isDraggingDivider = true;
    }

    if (panels.isDraggingDivider && input.leftHeld) {
        float usableHeight = (float)windowHeight - panels.dividerHeight;
        float newSplit = input.mouseY / usableHeight;
        panels.split = std::clamp(newSplit, 0.1f, 0.9f);
    }

    if (input.leftReleased) {
        panels.isDraggingDivider = false;
    }

    //Dragging panel width handle
    bool overWidthHandle = input.mouseX >= panelX - handleSize && input.mouseX <= panelX + handleSize &&
                           input.mouseY >= 0.0f && input.mouseY <= (float)windowHeight;
    if (input.leftDown && overWidthHandle) {
        panels.isDraggingWidth = true;
    }

    if (panels.isDraggingWidth && input.leftHeld) {
        float newWidth = baseW - input.mouseX;
        panels.panelWidth = std::clamp(newWidth, panels.minWidth, panels.maxWidth);
    }

    if (input.leftDown && !panels.isDraggingDivider && !panels.isDraggingWidth) {
        for (auto& entry : panels.entries) {
            bool over = 
                input.mouseX >= entry.x &&
                input.mouseX <= entry.x + entry.w &&
                input.mouseY >= entry.y &&
                input.mouseY <= entry.y + entry.h;

            if (!over) continue;

            for (auto e : em.getEntities()) {
                auto* st = em.getComponent<stateComponent>(e);
                if (st) st->isSelected = false;
            }

            auto* st = em.getComponent<stateComponent>(entry.entity);
            if (st) st->isSelected = true;

            em.bringToFront(entry.entity);

            break;
        }
    }

    if (input.leftReleased) {
        panels.isDraggingWidth = false;
    }

    //Properties tab inputs
    
}