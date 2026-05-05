#include "app.h"
#include "renderer.h"
#include "input.h"
#include "systems/nodeInteraction.h"
#include "systems/textInput.h"
#include <iostream>

//Initialise SDL, Window, Renderer and Icon
bool init(App& app) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL failed: " << SDL_GetError() << std::endl;
        return false;
    }

    TTF_Init();
    app.font = TTF_OpenFont("assets/fonts/VendSans-Regular.ttf", 16);
    
    if (!app.font) {
        std::cout << "Font failed: " << SDL_GetError() << std::endl;
    }

    app.icon = IMG_Load("assets/src/Plasma.png");
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

    SDL_StartTextInput(app.window);

    return true;
};

//Handles update loop
void update(App& app, double deltaTime) {
    updateCanvas(app.canvas, app.input, app.windowWidth, app.windowHeight, deltaTime);
    updatePanelsState(app.panels, app.EntityManager, app.input, app.windowWidth, app.windowHeight);
    updateUIState(app.input, app.ui, app.canvas, app.EntityManager, app.panels);
    updatePanels(app.panels, app.windowWidth, app.windowHeight);
    updateUILayout(app.ui, app.windowWidth, app.windowHeight, app.panels);
    updateToolButtons(app.ui);
    updateToolbarResize(app.ui, app.input, app.panels);
    updateNodeInteraction(app.input, app.EntityManager, app.canvas, app.ui, app.panels);
    updateInputRepeat(app.input, deltaTime);
    updateTextInput(app.input, app.EntityManager);
    updateEditingTimeout(app.EntityManager, deltaTime);
    updateTypingState(app.input, app.EntityManager);
}

//Handles render loop
void render(App& app) {
    beginFrame(app.renderer);
    renderCanvas(app.renderer, app.canvas, app.windowWidth, app.windowHeight);
    renderNodes(app.renderer, app.font, app.EntityManager, app.canvas, app.windowWidth, app.windowHeight);
    renderUI(app.renderer, app.ui, app.input, app.canvas);
    renderPanels(app.renderer, app.font, app.EntityManager, app.panels);
    endFrame(app.renderer);
}

//Handles clean up
void cleanUp(App& app) {
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    TTF_CloseFont(app.font);
    TTF_Quit();
    SDL_Quit();
}