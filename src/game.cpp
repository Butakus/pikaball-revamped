#include "game.hpp"
#include "SDL3/SDL.h"
#include <pikaball/controller/keyboard_controller.hpp>
#include <pikaball/controller/computer_controller.hpp>

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
  // By default, both players are controlled by the keyboard
  // controller_left_ = std::make_unique<KeyboardController>(FieldSide::Left);
  // controller_right_ = std::make_unique<KeyboardController>(FieldSide::Right);
  controller_left_ = std::make_unique<ComputerController>(FieldSide::Left);
  controller_right_ = std::make_unique<ComputerController>(FieldSide::Right);
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
      volley_view_->start();
    }
    break;
  case GameState::VolleyGame:
    // Send the current input state to the view
    const PhysicsView physics_view {*physics_};
    const PlayerInput input_left = controller_left_->update(physics_view);
    const PlayerInput input_right = controller_right_->update(physics_view);
    volley_view_->set_input(input_left, input_right);
    state_ = volley_view_->update();
    // Check if the view needs slow motion and change the FPS
    const unsigned int fps = volley_view_->slow_motion() ? slow_motion_fps_ : target_fps_;
    target_time_per_frame_ = ns_per_second / fps;
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
    const unsigned long start_time = SDL_GetTicksNS();
    step();
    const unsigned long end_time = SDL_GetTicksNS();

    const unsigned long sleep_time = target_time_per_frame_ - (end_time - start_time);
    SDL_DelayPrecise(sleep_time);
  }
}

void Game::handle_input() {
  // Enter / power-hit keys are handled by events to avoid repetitions
  PlayerInput player_input_left {};
  PlayerInput player_input_right {};
  menu_input_.enter_left = false;
  menu_input_.enter_right = false;

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
          player_input_left.power_hit = true;
          menu_input_.enter_left = true;
          break;
        case SDL_SCANCODE_RETURN:
        case SDL_SCANCODE_KP_ENTER:
          player_input_right.power_hit = true;
          menu_input_.enter_right = true;
          break;
        default:
          break;
      }
    }
  }

  // Forget about events and just grab a snapshot of the current keyboard state
  const bool* keys = SDL_GetKeyboardState(nullptr);

  // Convert keys from bool to a DirX/DirY object and set the player input directions
  player_input_left.direction_x =
    get_input_direction_x(keys[SDL_SCANCODE_D], keys[SDL_SCANCODE_G]);
  player_input_left.direction_y =
    get_input_direction_y(keys[SDL_SCANCODE_R], keys[SDL_SCANCODE_F]);
  player_input_right.direction_x =
    get_input_direction_x(keys[SDL_SCANCODE_LEFT], keys[SDL_SCANCODE_RIGHT]);
  player_input_right.direction_y =
    get_input_direction_y(keys[SDL_SCANCODE_UP], keys[SDL_SCANCODE_DOWN]);

  // Pass keyboard inputs to the keyboard controllers (if controllers are keyboards)
  if (const auto kb_left = dynamic_cast<KeyboardController*>(controller_left_.get())) {
    kb_left->set_input(player_input_left);
  }
  if (const auto kb_right = dynamic_cast<KeyboardController*>(controller_right_.get())) {
    kb_right->set_input(player_input_right);
  }

  menu_input_.up = keys[SDL_SCANCODE_UP] | keys[SDL_SCANCODE_R];
  menu_input_.down = keys[SDL_SCANCODE_DOWN] | keys[SDL_SCANCODE_F];
  menu_input_.enter = menu_input_.enter_left | menu_input_.enter_right;
}

} // namespace pika
