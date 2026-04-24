#include "app.h"

int main(int argc, char *argv[]) {
    App app;

    if (!init(app)) return 1;

    // logic for deltaTime
    Uint64 perfFrequency = SDL_GetPerformanceFrequency();
    Uint64 lastCounter = SDL_GetPerformanceCounter();

    while (app.running) {

        Uint64 currentCounter = SDL_GetPerformanceCounter();
        double deltaTime = (double)(currentCounter - lastCounter) / (double)(perfFrequency);
        lastCounter = currentCounter;

        
        eventHandler(app);
        update(app, deltaTime);
        render(app);
        
    }
    cleanUp(app);
    return 0;
}