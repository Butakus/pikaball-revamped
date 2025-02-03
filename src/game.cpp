#include "game.hpp"
#include "SDL3/SDL.h"

namespace pika {

void Game::run() {
  SDL_Log("Running stuff!");
  running_ = true;

  // Event data
  SDL_Event event;
  SDL_zero(event);  // Actually not required?

  while (running_) {
    // Get current input state from keyboard
    handle_input();

    // TODO: Update game state (logic)
    // Update surface content
    window_.render();
  }
}

void Game::handle_input() {
  // Event data
  SDL_Event event;
  SDL_zero(event);  // Actually not required?

  // PollEvent checks new events to process. Not thread safe.
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      running_ = false;
    }
  }

  // Forget about events and just grab a snapshot of the current keyboard state
  const bool* keys = SDL_GetKeyboardState(nullptr);
  player_input_1_.left = keys[SDL_SCANCODE_D];
  player_input_1_.right = keys[SDL_SCANCODE_G];
  player_input_1_.up = keys[SDL_SCANCODE_R];
  player_input_1_.down = keys[SDL_SCANCODE_F];
  player_input_1_.power_hit = keys[SDL_SCANCODE_Z];
  player_input_2_.left = keys[SDL_SCANCODE_LEFT];
  player_input_2_.right = keys[SDL_SCANCODE_RIGHT];
  player_input_2_.up = keys[SDL_SCANCODE_UP];
  player_input_2_.down = keys[SDL_SCANCODE_DOWN];
  player_input_2_.power_hit = keys[SDL_SCANCODE_RETURN] | keys[SDL_SCANCODE_KP_ENTER];

  menu_input_.up = keys[SDL_SCANCODE_UP] | keys[SDL_SCANCODE_R];
  menu_input_.down = keys[SDL_SCANCODE_DOWN] | keys[SDL_SCANCODE_F];
  menu_input_.enter =
    keys[SDL_SCANCODE_RETURN] | keys[SDL_SCANCODE_KP_ENTER] | keys[SDL_SCANCODE_Z];
}

} // namespace pika