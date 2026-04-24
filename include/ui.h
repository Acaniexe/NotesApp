#pragma once
#include "ecs.h"
#include "input.h"

enum class DockSide {
    Top,
    Bottom,
    Left,
    Right
};

enum class Orientation {
    Horizontal,
    Vertical
};

struct UI {
    float baseW = 0.0f;
    float baseH = 0.0f;

    float uiX = 0.0f;
    float uiY = 0.0f;
    float uiW = 0.0f;
    float uiH = 0.0f;

    DockSide dock = DockSide::Left;
    Orientation orientation = Orientation::Vertical;

    bool isCollapsed = false;

    bool isDragging = false;
    bool dragStarted = false;

    float dragOffsetX = 0.0f;
    float dragOffsetY = 0.0f;

    float clickStartX = 0.0f;
    float clickStartY = 0.0f;

    NodeType activeTool = NodeType::Note;
};

void updateUILayout(UI& ui, int windowWidth, int windowHeight);
void updateUIState(InputState& input, UI& ui);
bool isMouseOverUI(const UI& ui, float mx, float my);