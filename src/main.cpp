#include "app.h"

int main(int argc, char *argv[]) {
    App app;

    //Initalise application 
    if (!init(app)) return 1;

    // logic for deltaTime
    Uint64 perfFrequency = SDL_GetPerformanceFrequency();
    Uint64 lastCounter = SDL_GetPerformanceCounter();

    //Main application loop
    while (app.running) {

        //calculate frame delta time (seconds)
        Uint64 currentCounter = SDL_GetPerformanceCounter();
        double deltaTime = (double)(currentCounter - lastCounter) / (double)(perfFrequency);
        lastCounter = currentCounter;

        //Event, Update and render logic
        eventHandler(app);
        update(app, deltaTime);
        render(app);
        
    }
    cleanUp(app);
    return 0;
}