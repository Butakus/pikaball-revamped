#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>

#include <cstdint>
#include <exception>
#include <string>

#include "game.hpp"

namespace {

/**
 * Log a fatal error and show it to the player in an error dialog.
 * The dialog matters on Windows, where the GUI build has no console to show the log.
 * @param context What the game was doing (first line of the dialog)
 * @param message The error details (usually the exception message)
 */
void report_fatal_error(const char* context, const char* message) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s: %s", context, message);
    const std::string text = std::string(context) + ":\n\n" + message;
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, pika::window_name, text.c_str(), nullptr);
}

/**
 * Run an SDL callback body and turn any escaping exception into a reported
 * SDL_APP_FAILURE. Exceptions must not propagate into SDL's C code.
 * @param context Description used in the error dialog if the callback throws
 * @param callback The callback body, returning an SDL_AppResult
 */
template <typename Callback>
SDL_AppResult run_guarded(const char* context, Callback&& callback) {
    try {
        return callback();
    }
    catch (const std::exception& e) {
        report_fatal_error(context, e.what());
    }
    catch (...) {
        report_fatal_error(context, "Unknown error");
    }
    return SDL_APP_FAILURE;
}

} // namespace


SDL_AppResult SDL_AppInit(void **appstate,
                          [[maybe_unused]] int argc,
                          [[maybe_unused]] char **argv) {
    // Setup logger verbosity
    #ifdef PIKA_DEBUG
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
    #else
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_WARN);
    #endif
    // Create a Game object that will be passed back to each callback.
    return run_guarded("The game could not start", [appstate] {
        *appstate = new pika::Game;
        return SDL_APP_CONTINUE;
    });
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    return run_guarded("The game stopped because of an error", [appstate] {
        const std::uint64_t start_time = SDL_GetTicksNS();
        pika::Game& game = *static_cast<pika::Game *>(appstate);
        // Execute game update step and draw
        game.step();
        const std::uint64_t elapsed = SDL_GetTicksNS() - start_time;

        // Sleep for the rest of the frame.
        // Skip the sleep if the frame took longer than its duration.
        const std::uint64_t frame_duration = game.get_frame_duration();
        if (elapsed < frame_duration) {
            SDL_DelayPrecise(frame_duration - elapsed);
        }
        return SDL_APP_CONTINUE;
    });
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    return run_guarded("The game stopped because of an error", [appstate, event] {
        if(event) {
            const auto game = static_cast<pika::Game*>(appstate);
            game->handle_event(event);
            if(event->type == SDL_EVENT_QUIT)
            {
                return SDL_APP_SUCCESS;
            }
        }
        return SDL_APP_CONTINUE;
    });
}

void SDL_AppQuit(void *appstate, [[maybe_unused]] SDL_AppResult result)
{
    const auto game = static_cast<pika::Game*>(appstate);
    delete game;
}
