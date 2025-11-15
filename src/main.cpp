
#include "SDL3/SDL.h"
#include "application/application.h"

int main(int argc, char *argv[])
{

    SDL_Log("Program started with %d argument(s):", argc);
    for (int i = 0; i < argc; i++) 
    {
        SDL_Log("%s", argv[i]);
    }
    
    Wrench::Application app;

    if (app.init())
    {
        app.run();
        app.exit();
    }
    else
    {
        return -1;
    }

    return 0;
}
