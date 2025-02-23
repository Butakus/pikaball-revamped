#include "game.hpp"
#include "SDL3/SDL.h"

namespace pika {

Game::Game() {
  physics_ = std::make_unique<Physics>(),
  intro_view_ = std::make_unique<view::IntroView>(
    window_.get_renderer(), window_.get_sprite_sheet());
  menu_view_ = std::make_unique<view::MenuView>(
    window_.get_renderer(), window_.get_sprite_sheet());
  volley_view_ = std::make_unique<view::VolleyView>(
    window_.get_renderer(),
    window_.get_sprite_sheet(),
    physics_.get()
  );
}


void Game::step() {
  // Get current input state from keyboard
  handle_input();

  switch (state_) {
  case GameState::Intro:
    intro_view_->set_input(menu_input_);
    state_ = intro_view_->update();
    // Check transition
    if (state_ == GameState::Menu) {
      menu_view_->start();
    }
    break;
  case GameState::Menu:
    menu_view_->set_input(menu_input_);
    state_ = menu_view_->update();
    if (state_ == GameState::VolleyGame) {
      const view::MenuPlayerSelection selection = menu_view_->get_selection();
      SDL_Log("Player selection: %d", selection);
      physics_->restart(false, false);  // Is computer is always false for now
      volley_view_->start();
    }
    break;
  case GameState::VolleyGame:
    // Send the current input state to the view
    volley_view_->set_input(player_input_left_, player_input_right_);
    state_ = volley_view_->update();
    // Check if the view needs slow motion and change the FPS
    const Uint64 fps = volley_view_->slow_motion() ? slow_motion_fps_ : target_fps_;
    target_time_per_frame_ = SDL_NS_PER_SECOND / fps;
    if (state_ == GameState::Intro) {
      intro_view_->start();
    }
    break;
  }
}

void Game::run() {
  SDL_Log("Running stuff!");
  running_ = true;

  // Initialize state and window
  state_ = GameState::Intro;
  intro_view_->start();

  while (running_) {
    const Uint64 start_time = SDL_GetTicksNS();
    step();
    const Uint64 end_time = SDL_GetTicksNS();

    const Uint64 sleep_time = target_time_per_frame_ - (end_time - start_time);
    SDL_DelayPrecise(sleep_time);
  }
}

void Game::handle_input() {
  // Enter / power-hit keys are handled by events to avoid repetitions
  player_input_left_.power_hit = false;
  player_input_right_.power_hit = false;
  menu_input_.enter = false;

  // Event data
  SDL_Event event {};

  // Check and process all new events. Not thread safe.
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      running_ = false;
      break;
    }
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
      switch (event.key.scancode) {
        case SDL_SCANCODE_Z:
          player_input_left_.power_hit = true;
          menu_input_.enter = true;
          break;
        case SDL_SCANCODE_RETURN:
        case SDL_SCANCODE_KP_ENTER:
          player_input_right_.power_hit = true;
          menu_input_.enter = true;
          break;
        default:
          break;
      }
    }
  }

  // Forget about events and just grab a snapshot of the current keyboard state
  const bool* keys = SDL_GetKeyboardState(nullptr);
  player_input_left_.left = keys[SDL_SCANCODE_D];
  player_input_left_.right = keys[SDL_SCANCODE_G];
  player_input_left_.up = keys[SDL_SCANCODE_R];
  player_input_left_.down = keys[SDL_SCANCODE_F];
  player_input_right_.left = keys[SDL_SCANCODE_LEFT];
  player_input_right_.right = keys[SDL_SCANCODE_RIGHT];
  player_input_right_.up = keys[SDL_SCANCODE_UP];
  player_input_right_.down = keys[SDL_SCANCODE_DOWN];

  menu_input_.up = keys[SDL_SCANCODE_UP] | keys[SDL_SCANCODE_R];
  menu_input_.down = keys[SDL_SCANCODE_DOWN] | keys[SDL_SCANCODE_F];
}

} // namespace pika