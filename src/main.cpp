#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>

#include "game.hpp"

// int main(int argc, char **argv) {
//     pika::Game game;
//     game.run();
//     return 0;
// }

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    // Create a Game object that will be passed back to each callback:
    *appstate = new pika::Game;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    const unsigned long start_time = SDL_GetTicksNS();
    pika::Game& game = *static_cast<pika::Game *>(appstate);
    // Execute game update step and draw
    game.step();
    const unsigned long end_time = SDL_GetTicksNS();

    const unsigned long sleep_time = game.get_frame_time() - (end_time - start_time);
    if (sleep_time > 0) {
        SDL_DelayPrecise(sleep_time);
    }
    return SDL_APP_CONTINUE;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if(event) {
        const auto game = static_cast<pika::Game*>(appstate);
        game->handle_event(event);
        if(event->type == SDL_EVENT_QUIT)
        {
            return SDL_APP_SUCCESS;
        }
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, [[maybe_unused]] SDL_AppResult result)
{
    const auto game = static_cast<pika::Game*>(appstate);
    delete game;
}
