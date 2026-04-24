#include "app.h"
#include "renderer.h"
#include <iostream>

// logic to initialise SDL && create window + renderer
bool init(App& app) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL failed: " << SDL_GetError() << std::endl;
        return false;
    }

    app.window = SDL_CreateWindow("Testing", app.windowWidth, app.windowHeight, SDL_WINDOW_RESIZABLE);
    if (!app.window) {
        std::cout << "Window failed: " << SDL_GetError() << std::endl;
        return false;
    }

    app.renderer = SDL_CreateRenderer(app.window, NULL);
    if (!app.renderer) {
        std::cout << "Renderer failed: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_SetRenderVSync(app.renderer, 1);
    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);

    return true;
};

void update(App& app, double deltaTime) {
    if (!app.ecsTestDone) {
        Entity e = app.EntityManager.createEntity();

        std::cout << "Entity created: " << e.id << std::endl;

        app.EntityManager.addComponent<PositionComponent>(e, 100.f, 200.f);

        auto* pos = app.EntityManager.getComponent<PositionComponent>(e);

        std::cout << "Position: " << pos->x << ", " << pos->y << std::endl;

        app.ecsTestDone = true;
    }
    updateCanvas(app.canvas, app.input, app.windowWidth, app.windowHeight, deltaTime);
    updateUILayout(app.ui, app.windowWidth, app.windowHeight);
    updateUIState(app.input, app.ui);
    handleNodeInput(app.input, app.EntityManager, app.ui, app.canvas);
}

void render(App& app) {
    beginFrame(app.renderer);
    renderCanvas(app.renderer, app.canvas, app.windowWidth, app.windowHeight);
    renderNodes(app.renderer, app.EntityManager, app.canvas, app.windowWidth, app.windowHeight);
    renderUI(app.renderer, app.ui, app.input);
    endFrame(app.renderer);
}

void cleanUp(App& app) {
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
}