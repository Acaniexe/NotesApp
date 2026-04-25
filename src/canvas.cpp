#include "canvas.h"
#include "ui.h"
#include "ecs.h"
#include <cmath>
#include <iostream>

Vec2 screenToWorld(const Canvas& canvas, float sx, float sy) {
    return {
        canvas.cameraX + sx / canvas.zoom,
        canvas.cameraY + sy / canvas.zoom
    };
}

Vec2 worldToScreen(const Canvas& canvas, float wx, float wy) {
    return {
        (wx - canvas.cameraX) * canvas.zoom,
        (wy - canvas.cameraY) * canvas.zoom
    };
}

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

/*void handleNodeInput(const InputState& input, EntityManager& entityManager, UI& ui, const Canvas& canvas) {
    if (isMouseOverUI(ui, input.mouseX, input.mouseY) || ui.isDragging) return;

    if (input.leftDown) {
        Vec2 world = screenToWorld(canvas, input.mouseX, input.mouseY);

        std::cout << "CLICK detected at screen: "
            << input.mouseX << ", " << input.mouseY << "\n";

        std::cout << "Active tool: " << (int)ui.activeTool << "\n";

        createNode(entityManager, ui.activeTool, world.x, world.y);
    }
}*/