#include "app.h"
#include <cmath>
#include <iostream>

void eventHandler(App& app) {
    SDL_Event event;
    // reset at start of everyframe
    app.input.windowResized = false;
    app.input.leftDown = false;
    app.input.leftReleased = false;
    app.input.rightDown = false;
    app.input.rightReleased = false;
    app.input.zoom = 0.0f;

    // logic for QUIT && windowResize
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                app.running = false;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                app.windowWidth = event.window.data1;
                app.windowHeight = event.window.data2;
                app.input.windowResized = true;
                std::cout << "Window width: " << app.windowWidth << std::endl;
                std::cout << "Window height: " << app.windowHeight << std::endl;
                break;

            case SDL_EVENT_MOUSE_MOTION:
                app.input.mouseX = event.motion.x;
                app.input.mouseY = event.motion.y;
                break;

            case SDL_EVENT_MOUSE_WHEEL:
                app.input.zoom = event.wheel.y * 0.1f;
                app.input.zoom += event.wheel.x * 0.1f;
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    app.input.leftDown = true;
                    app.input.leftHeld = true;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT) {
                    app.input.rightDown = true;
                    app.input.rightHeld = true;
                }
                break;
            
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    app.input.leftReleased = true;
                    app.input.leftHeld = false;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT) {
                    app.input.rightReleased = true;
                    app.input.rightHeld = false;
                }
                break;

            case SDL_EVENT_KEY_DOWN:
                if (event.key.repeat == 0) {
                    if (event.key.scancode == SDL_SCANCODE_SEMICOLON) {
                        app.input.dockCollapsePressed = true;
                    } else if (event.key.scancode == SDL_SCANCODE_N) {
                        app.input.noteToolPressed = true;
                    } else if (event.key.scancode == SDL_SCANCODE_T) {
                        app.input.textToolPressed = true;
                    } else if (event.key.scancode == SDL_SCANCODE_I) {
                        app.input.imageToolPressed = true;
                    } else if (event.key.scancode == SDL_SCANCODE_D) {
                        app.input.todoToolPressed = true;
                    }
                }
                break;

            case SDL_EVENT_KEY_UP:
                if (event.key.scancode == SDL_SCANCODE_SEMICOLON) {
                    app.input.dockCollapsePressed = false;
                } else if (event.key.scancode == SDL_SCANCODE_N) {
                    app.input.noteToolPressed= false;
                } else if (event.key.scancode == SDL_SCANCODE_T) {
                    app.input.textToolPressed = false;
                } else if (event.key.scancode == SDL_SCANCODE_I) {
                    app.input.imageToolPressed = false;
                } else if (event.key.scancode == SDL_SCANCODE_D) {
                    app.input.todoToolPressed = false;
                }
                break;
        }
    }

    const bool* keystate = SDL_GetKeyboardState(NULL);
    //app.input.dockTop = keystate[SDL_SCANCODE_J];
}