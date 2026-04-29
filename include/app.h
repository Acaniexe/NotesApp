#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "input.h"
#include "canvas.h"
#include "ui.h"
#include "ecs.h"

//App state
struct App {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Surface* icon = nullptr;

    //Window state
    int windowWidth = 900;
    int windowHeight = 500;
    bool running = true;

    //App. References
    InputState input;
    Canvas canvas;
    UI ui;
    Panels panels;
    EntityManager EntityManager;
};

//App functions
bool init(App& app);
void eventHandler(App& app);
void update(App& app, double deltaTime);
void render(App& app);
void cleanUp(App& app);