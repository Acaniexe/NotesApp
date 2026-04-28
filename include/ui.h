#pragma once
#include "ecs.h"
#include "input.h"
#include "canvas.h"
#include <vector>

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

struct ToolButton {
    NodeType type;
    float x, y, w, h;
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

    bool isPinned = true;
    bool isHovered = false;

    bool isDragging = false;
    bool dragStarted = false;

    float dragOffsetX = 0.0f;
    float dragOffsetY = 0.0f;

    float clickStartX = 0.0f;
    float clickStartY = 0.0f;

    bool isDraggingTool = false;
    NodeType draggingToolType;
    float toolDragX = 0.0f;
    float toolDragY = 0.0f;

    NodeType activeTool = NodeType::Note;
    std::vector<ToolButton> toolButtons;
};

struct Panel {
    float x, y, w, h;
};

struct Panels {
    Panel top;
    Panel bottom;

    float split = 0.5f;
    float dividerHeight = 6.0f;

    bool isDraggingDivider = false;
};

void updateUILayout(UI& ui, int windowWidth, int windowHeight);
void updateToolButtons(UI& ui);
void updateUIState(InputState& input, UI& ui, Canvas& canvas, EntityManager& entityManager);
void updatePanels(Panels& panels, int windowWidth, int windowHeight);
void updatePanelsState(Panels& panels, InputState& input, int windowWidth, int windowHeight);
bool isMouseOverUI(const UI& ui, float mx, float my);