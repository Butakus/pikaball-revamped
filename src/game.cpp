#include "game.hpp"
#include "SDL3/SDL.h"

namespace pika {

Game::Game() {
  intro_view_ = std::make_unique<view::IntroView>(window_.get_sprite_sheet());
  menu_view_ = std::make_unique<view::MenuView>(window_.get_sprite_sheet());
  volley_view_ = std::make_unique<view::VolleyView>(window_.get_sprite_sheet());
}


void Game::step() {
  // Get current input state from keyboard
  handle_input();

  switch (state_) {
  case GameState::Intro:
    if (frame_counter_ >= view::IntroView::max_frames || menu_input_.enter) {
      state_ = GameState::Menu;
      frame_counter_ = 0;
      window_.set_view(menu_view_.get());
      return;  // TODO: Skip render?
    }
    break;
  case GameState::Menu:
    menu_view_->set_input(menu_input_);
    if (menu_view_->is_finished()) {
      state_ = GameState::Round;
    }
    break;
  case GameState::Round:
    window_.set_view(volley_view_.get());  // TODO: This is only needed at the state transition
    // Send the current input state to the view
    volley_view_->set_input(player_input_1_, player_input_2_);
    break;
  default:
    // TODO
    window_.set_view(nullptr);
    break;
  }
  // Update surface content
  window_.render();
  frame_counter_++;
}

void Game::run() {
  SDL_Log("Running stuff!");
  running_ = true;

  // Initialize state and window
  state_ = GameState::Intro;
  window_.set_view(intro_view_.get());
  frame_counter_ = 0;

  while (running_) {
    const Uint64 start_time = SDL_GetTicksNS();
    step();
    const Uint64 end_time = SDL_GetTicksNS();

    const Uint64 sleep_time = target_time_per_frame_ - (end_time - start_time);
    SDL_DelayPrecise(sleep_time);
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