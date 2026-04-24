#include "ui.h"
#include <algorithm>

void updateUILayout(UI& ui, int windowWidth, int windowHeight) {
    ui.baseW = (float)windowWidth;
    ui.baseH = (float)windowHeight;

    if (ui.isDragging) return;

    ui.orientation = (ui.dock == DockSide::Top || ui.dock == DockSide::Bottom) ? Orientation::Horizontal : Orientation::Vertical;

    float dragSize = 25.0f;
   
    float length = (ui.orientation == Orientation::Horizontal) ? ui.baseW : ui.baseH;

    float thicknessPercent = (ui.orientation == Orientation::Horizontal) ? 0.12f : 0.08; //0.12f; 

    float thickness = (ui.orientation == Orientation::Horizontal) ? ui.baseH * thicknessPercent : ui.baseW * thicknessPercent;

    float minThickness = 60.0f;
    float maxThickness = 100.0f;
    thickness = std::clamp(thickness, minThickness, maxThickness);

    if (ui.dock == DockSide::Top || ui.dock == DockSide::Bottom) {
        ui.uiW = length;
        ui.uiH = thickness;
    } else {
        ui.uiW = thickness;
        ui.uiH = length;
    }

    switch (ui.dock) {
        case DockSide::Top:
            ui.uiX = 0.0f;
            ui.uiY = 0.0f;
            break;

        case DockSide::Bottom:
            ui.uiX = 0.0f;
            ui.uiY = ui.baseH - ui.uiH;
            break;

        case DockSide::Left:
            ui.uiX = 0.0f;
            ui.uiY = 0.0f;
            break;

        case DockSide::Right:
            ui.uiX = ui.baseW - ui.uiW;
            ui.uiY = 0.0f;
            break;
    }
}

void updateUIState(InputState& input, UI& ui) {

    if (input.noteToolPressed) ui.activeTool = NodeType::Note;
    if (input.textToolPressed) ui.activeTool = NodeType::Text;
    if (input.imageToolPressed) ui.activeTool = NodeType::Image;
    if (input.todoToolPressed) ui.activeTool = NodeType::ToDo;

    if (input.dockCollapsePressed) {
        ui.isCollapsed = !ui.isCollapsed;
        input.dockCollapsePressed = false;
    }

    float dragSize = 25.0f;

    bool mouseOverDragZone = false;

    if (ui.orientation == Orientation::Vertical) {
        mouseOverDragZone =
            input.mouseX >= ui.uiX &&
            input.mouseX <= ui.uiX + ui.uiW &&
            input.mouseY >= ui.uiY &&
            input.mouseY <= ui.uiY + dragSize;
    } else {
        mouseOverDragZone =
            input.mouseX >= ui.uiX &&
            input.mouseX <= ui.uiX + dragSize &&
            input.mouseY >= ui.uiY &&
            input.mouseY <= ui.uiY + ui.uiH;
    }

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

        if (!ui.dragStarted) {
            if (dx * dx + dy * dy > dragThreshold * dragThreshold) {
                ui.dragStarted = true;
            }
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
            if (input.mouseY < margin) {
                ui.dock = DockSide::Top;
            }
            else if (input.mouseY > ui.baseH - margin) {
                ui.dock = DockSide::Bottom;
            }
            else if (input.mouseX < margin) {
                ui.dock = DockSide::Left;
            }
            else if (input.mouseX > ui.baseW - margin) {
                ui.dock = DockSide::Right;
            }
        }
    }
}

bool isMouseOverUI(const UI& ui, float mx, float my) {
    return mx >= ui.uiX &&
           mx <= ui.uiX + ui.uiW &&
           my >= ui.uiY &&
           my <= ui.uiY + ui.uiH;
}