#pragma once
#include "ecs.h"
#include "input.h"
#include "canvas.h"
#include <vector>

//Docking sids for UI toolbar
enum class DockSide {
    Top,
    Bottom,
    Left,
    Right
};

//Layout Orientation for UI toolbar
enum class Orientation {
    Horizontal,
    Vertical
};

//Tool Button in toolbar
struct ToolButton {
    NodeType type;
    float x, y, w, h;
};

//Main toolbar state
struct UI {
    //Window reference size
    float baseW = 0.0f;
    float baseH = 0.0f;

    //UI toolbar rectangle size
    float uiX = 0.0f;
    float uiY = 0.0f;
    float uiW = 0.0f;
    float uiH = 0.0f;

    //Handles Docking and Orientation
    DockSide dock = DockSide::Left;
    Orientation orientation = Orientation::Vertical;

    //Pin/Unpin states
    bool isPinned = true;
    bool isHovered = false;

    //Dragging states
    bool isDragging = false;
    bool dragStarted = false;

    float dragOffsetX = 0.0f;
    float dragOffsetY = 0.0f;

    float clickStartX = 0.0f;
    float clickStartY = 0.0f;

    //Tool Dragging 
    bool isDraggingTool = false;
    NodeType draggingToolType;
    float toolDragX = 0.0f;
    float toolDragY = 0.0f;

    NodeType activeTool = NodeType::Note;
    std::vector<ToolButton> toolButtons;

    //Toolbar thickness for resize
    float uiThickness = 0.0f;
    float minThickness = 60.0f;
    float maxThickness = 150.0f;
    bool isDraggingThickness = false;
};

//Panel state
struct Panel {
    float x, y, w, h;
};

struct PanelEntry {
    Entity entity;
    float x, y, w, h;
};

enum class PropertyKind {
    String,
    Float,
    Int
};

struct PanelPropertyEntry {
    Entity entity;
    std::string label;
    std::string value;
    PropertyKind kind;
    PropertyField field;
    float x, y, w, h;
};

//Panels state
struct Panels {
    Panel top;
    Panel bottom;

    float split = 0.5f;
    float dividerHeight = 6.0f;

    float panelWidth = 0.0f;
    float minWidth = 125.0f;
    float maxWidth = 250.0f;

    bool isDraggingDivider = false;
    bool isDraggingWidth = false;

    std::vector<PanelEntry> entries;
    std::vector<PanelPropertyEntry> propertyEntries;
};

//UI functions
void updateUILayout(UI& ui, int windowWidth, int windowHeight, const Panels& panels);
void updateToolButtons(UI& ui);
void updateUIState(InputState& input, UI& ui, Canvas& canvas, EntityManager& entityManager, const Panels& panels);
void updateToolbarResize(UI& ui, InputState& input, const Panels& panels);
void updatePanels(Panels& panels, int windowWidth, int windowHeight);
void updatePanelsState(Panels& panels, EntityManager& em, InputState& input, int windowWidth, int windowHeight);
bool isMouseOverUI(const UI& ui, float mx, float my);