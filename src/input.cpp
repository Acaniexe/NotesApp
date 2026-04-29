#include "app.h"
#include <cmath>
#include <iostream>

//Converts SDL events into per-frame input states
void eventHandler(App& app) {
    SDL_Event event;
    // reset at start of everyframe
    app.input.windowResized = false;
    app.input.leftDown = false;
    app.input.leftReleased = false;
    app.input.rightDown = false;
    app.input.rightReleased = false;
    app.input.ctrlDown = false;
    app.input.ctrlReleased = false;
    app.input.zoom = 0.0f;

    //Event queue
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            //Application quit
            case SDL_EVENT_QUIT:
                app.running = false;
                break;

            //Window resize
            case SDL_EVENT_WINDOW_RESIZED:
                app.windowWidth = event.window.data1;
                app.windowHeight = event.window.data2;
                app.input.windowResized = true;
                std::cout << "Window width: " << app.windowWidth << std::endl;
                std::cout << "Window height: " << app.windowHeight << std::endl;
                break;

            //Mouse movement (screen space)
            case SDL_EVENT_MOUSE_MOTION:
                app.input.mouseX = event.motion.x;
                app.input.mouseY = event.motion.y;
                break;
            
            //Mouse wheel for zoom
            case SDL_EVENT_MOUSE_WHEEL:
                app.input.zoom = event.wheel.y * 0.1f;
                app.input.zoom += event.wheel.x * 0.1f;
                break;

            //Mouse button press
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

            //Mouse button release
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

            //Keyboard key press 
            case SDL_EVENT_KEY_DOWN:
                if (event.key.repeat == 0) {
                    if (event.key.scancode == SDL_SCANCODE_SEMICOLON) {
                        app.input.dockCollapsePressed = true;
                    }
                    else if (event.key.scancode == SDL_SCANCODE_DELETE) {
                        app.input.delPressed = true;
                    }
                }
                break;

            //Keyboard key release
            case SDL_EVENT_KEY_UP:
                if (event.key.scancode == SDL_SCANCODE_SEMICOLON) {
                    app.input.dockCollapsePressed = false;
                }
                else if (event.key.scancode == SDL_SCANCODE_DELETE) {
                    app.input.delPressed = false;
                }
                break;
        }
    }

    //Keyboard key states (held keys)
    const bool* keystate = SDL_GetKeyboardState(NULL);
    app.input.ctrlHeld = keystate[SDL_SCANCODE_LCTRL] || keystate[SDL_SCANCODE_RCTRL];

    //Edge detection for ctrl
    app.input.ctrlDown = app.input.ctrlHeld && !app.input.prevCtrlHeld;
    app.input.ctrlReleased = !app.input.ctrlHeld && app.input.prevCtrlHeld;

    app.input.prevCtrlHeld = app.input.ctrlHeld;
}