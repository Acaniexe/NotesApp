#include "app.h"
#include "renderer.h"
#include "systems/nodeInteraction.h"
#include <iostream>

// logic to initialise SDL && create window + renderer
bool init(App& app) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL failed: " << SDL_GetError() << std::endl;
        return false;
    }

    app.icon = IMG_Load("assets/Plasma.png");
    if (!app.icon) {
        std::cout << "Icon failed: " << SDL_GetError() << std::endl;
    }

    app.window = SDL_CreateWindow("Plasma Notes", app.windowWidth, app.windowHeight, SDL_WINDOW_RESIZABLE);
    if (!app.window) {
        std::cout << "Window failed: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_SetWindowIcon(app.window, app.icon);
    SDL_DestroySurface(app.icon);

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
    updateCanvas(app.canvas, app.input, app.windowWidth, app.windowHeight, deltaTime);
    updateUILayout(app.ui, app.windowWidth, app.windowHeight);
    updateToolButtons(app.ui);
    updateUIState(app.input, app.ui, app.canvas, app.EntityManager);
    updateNodeInteraction(app.input, app.EntityManager, app.canvas, app.ui);
}

void render(App& app) {
    beginFrame(app.renderer);
    renderCanvas(app.renderer, app.canvas, app.windowWidth, app.windowHeight);
    renderNodes(app.renderer, app.EntityManager, app.canvas, app.windowWidth, app.windowHeight);
    renderUI(app.renderer, app.ui, app.input, app.canvas);
    endFrame(app.renderer);
}

void cleanUp(App& app) {
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
}