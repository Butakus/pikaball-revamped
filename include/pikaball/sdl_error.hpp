#ifndef PIKA_SDL_ERROR_HPP
#define PIKA_SDL_ERROR_HPP

#include <stdexcept>
#include <string>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"

namespace pika {

/**
 * Custom exception for a failed SDL (or SDL_mixer / SDL_ttf) call.
 * The message is "<what>: <SDL_GetError()>", so it carries the actual reason.
 * It is also logged here, because the exception text may only reach the player in an error dialog.
 * Call it right after the failing SDL call, before anything else can overwrite the SDL error.
 *
 * Usage: throw sdl_error("Failed to create window");
 * @param what Description of the operation that failed
 * @return A std::runtime_error with the full message (to be thrown by the caller)
 */
[[nodiscard]] inline std::runtime_error sdl_error(const std::string& what) {
  std::string message = what;
  if (const char* detail = SDL_GetError(); detail != nullptr && detail[0] != '\0') {
    message += ": ";
    message += detail;
  }
  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
  return std::runtime_error(message);
}

} // namespace pika

#endif // PIKA_SDL_ERROR_HPP
