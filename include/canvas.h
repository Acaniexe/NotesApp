#pragma once
#include "SDL3/SDL.h"
#include "input.h"

class EntityManager;

struct UI;

struct Canvas {
    float cameraX = 0.0f;
    float cameraY = 0.0f;

    float zoom = 1.0f;

    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;
};

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

Vec2 screenToWorld(const Canvas& canvas, float sx, float sy);
Vec2 worldToScreen(const Canvas& canvas, float wx, float wy);
void updateCanvas(Canvas& canvas, const InputState& input, int windowWidth, int windowHeight, double deltaTime);
void handleNodeInput(const InputState& input, EntityManager& entityManager, UI& ui, const Canvas& canvas);