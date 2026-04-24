#pragma once
#include <SDL3/SDL.h>
#include "input.h"
#include "canvas.h"
#include "ui.h"
#include "ecs.h"

struct App {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    int windowWidth = 900;
    int windowHeight = 500;
    bool running = true;
    bool ecsTestDone = false;

    InputState input;
    Canvas canvas;
    UI ui;
    EntityManager EntityManager;
};

bool init(App& app);
void eventHandler(App& app);
void update(App& app, double deltaTime);
void render(App& app);
void cleanUp(App& app);