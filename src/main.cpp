#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>

#include <charconv>
#include <cstdint>
#include <exception>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

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

/**
 * Parse an unsigned number in [min, max] from a command line argument.
 * @throws std::invalid_argument if the text is not a number in range
 */
template <typename T>
T parse_number(const std::string_view text, const T min, const T max, const char* what) {
    unsigned long value {0};
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size() || value < min || value > max) {
        throw std::invalid_argument("Invalid " + std::string(what) + ": '" + std::string(text) + "'");
    }
    return static_cast<T>(value);
}

/**
 * Parse the network game options. Temporary for development, until the game has menus for it:
 *   --host <port>            Host a network game on the given UDP port
 *   --join <address:port>    Join the network game hosted at address:port
 *   --delay <frames>         Input delay in frames (host only, default 2)
 * @return The network settings, or nothing for a local game
 * @throws std::invalid_argument if the options are not valid
 */
std::optional<pika::net::NetConfig> parse_net_options(const int argc, char** argv) {
    std::optional<pika::net::NetConfig> config;
    std::optional<std::uint8_t> delay;
    for (int i = 1; i < argc; ++i) {
        const std::string_view option = argv[i];
        if (option != "--host" && option != "--join" && option != "--delay") {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Ignoring unknown option '%s'", argv[i]);
            continue;
        }
        if (i + 1 >= argc) {
            throw std::invalid_argument("Missing value after " + std::string(option));
        }
        const std::string_view value = argv[++i];

        if (option == "--delay") {
            delay = parse_number<std::uint8_t>(value, 1, 15, "input delay");
            continue;
        }
        if (config) {
            throw std::invalid_argument("Use only one of --host and --join");
        }
        config.emplace();
        if (option == "--host") {
            config->role = pika::net::NetConfig::Role::Host;
            config->port = parse_number<std::uint16_t>(value, 1, 65535, "port");
        } else {
            // The port is after the last ':'
            const auto colon = value.rfind(':');
            if (colon == std::string_view::npos) {
                throw std::invalid_argument("Use --join <address:port>");
            }
            config->role = pika::net::NetConfig::Role::Guest;
            config->address = value.substr(0, colon);
            config->port = parse_number<std::uint16_t>(value.substr(colon + 1), 1, 65535, "port");
        }
    }
    if (delay) {
        if (!config || config->role != pika::net::NetConfig::Role::Host) {
            throw std::invalid_argument("--delay can only be used with --host");
        }
        config->input_delay = *delay;
    }
    return config;
}

} // namespace


SDL_AppResult SDL_AppInit(void **appstate,
                          int argc,
                          char **argv) {
    // Setup logger verbosity
    #ifdef PIKA_DEBUG
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
    #else
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_WARN);
    #endif
    // Create a Game object that will be passed back to each callback.
    return run_guarded("The game could not start", [appstate, argc, argv] {
        *appstate = new pika::Game(parse_net_options(argc, argv));
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
