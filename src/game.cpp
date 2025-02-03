#include "game.hpp"
#include "SDL3/SDL.h"

namespace pika {

void Game::run() {
  SDL_Log("Running stuff!");
  bool quit = false;

  // Event data
  SDL_Event event;
  SDL_zero(event);  // Actually not required?

  while (!quit) {
    // PollEvent checks new events to process. Not thread safe.
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }

    // Update surface content
    window_.render();
  }
}

} // namespace pika