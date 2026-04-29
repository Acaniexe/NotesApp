#include "canvas.h"
#include "ui.h"
#include "ecs.h"
#include <cmath>
#include <iostream>

//Screen space -> World space conversion
Vec2 screenToWorld(const Canvas& canvas, float sx, float sy) {
    return {
        canvas.cameraX + sx / canvas.zoom,
        canvas.cameraY + sy / canvas.zoom
    };
}

//World space -> Screen space conversion
Vec2 worldToScreen(const Canvas& canvas, float wx, float wy) {
    return {
        (wx - canvas.cameraX) * canvas.zoom,
        (wy - canvas.cameraY) * canvas.zoom
    };
}

//Canvas camera logic
void updateCanvas(Canvas& canvas, const InputState& input, int windowWidth, int windowHeight, double deltaTime) {
    if (input.zoom != 0.0f) {
        float mouseX = input.mouseX;
        float mouseY = input.mouseY;

        float worldX = canvas.cameraX + mouseX / canvas.zoom;
        float worldY = canvas.cameraY + mouseY / canvas.zoom;

        canvas.zoom *= (1.0f + input.zoom);

        if (canvas.zoom < 0.1f) canvas.zoom = 0.1f;
        if (canvas.zoom > 5.0f) canvas.zoom = 5.0f;

        canvas.cameraX = worldX - mouseX / canvas.zoom;
        canvas.cameraY = worldY - mouseY / canvas.zoom;
    }

    if (input.rightHeld) {

        float deltaX = input.mouseX - canvas.lastMouseX;
        float deltaY = input.mouseY - canvas.lastMouseY;

        float invZoom = 1.0f / canvas.zoom;

        canvas.cameraX -= deltaX * invZoom;
        canvas.cameraY -= deltaY * invZoom;
    }

    canvas.lastMouseX = input.mouseX;
    canvas.lastMouseY = input.mouseY;
}