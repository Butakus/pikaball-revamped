#include "game.hpp"
#include "SDL3/SDL.h"
#include <pikaball/controller/keyboard_controller.hpp>
#include <pikaball/controller/computer_controller.hpp>

namespace pika {

Game::Game() {
  physics_ = std::make_unique<Physics>(),
  intro_view_ = std::make_unique<view::IntroView>(
    sdl_sys_.get_renderer(), sdl_sys_.get_sprite_sheet());
  menu_view_ = std::make_unique<view::MenuView>(
    sdl_sys_.get_renderer(), sdl_sys_.get_sprite_sheet());
  volley_view_ = std::make_unique<view::VolleyView>(
    sdl_sys_.get_renderer(), sdl_sys_.get_sprite_sheet());
  // By default, both players are controlled by the keyboard
  // controller_left_ = std::make_unique<KeyboardController>(FieldSide::Left);
  controller_right_ = std::make_unique<KeyboardController>(FieldSide::Right);
  controller_left_ = std::make_unique<ComputerController>(FieldSide::Left);
  // controller_right_ = std::make_unique<ComputerController>(FieldSide::Right);
}

void Game::step() {
  // Get current input state from keyboard
  handle_input();

  switch (state_) {
  case GameState::Intro:
    intro_state();
    break;
  case GameState::Menu:
    menu_state();
    break;
  case GameState::VolleyGame:
    // Send the current input state to the view
    // TODO: Decide where to get input from controllers. Here or after render?
    input_left_ = controller_left_->on_update(PhysicsView(*physics_));
    input_right_ = controller_right_->on_update(PhysicsView(*physics_));
    // volley_view_->set_input(input_left_, input_right_);
    // state_ = volley_view_->update();  // The physics object is updated inside this view
    volley_state();
    // Check if the view needs slow motion and change the FPS
    const unsigned int fps = slow_motion_ ? slow_motion_fps_ : target_fps_;
    target_time_per_frame_ = ns_per_second / fps;
    break;
  }

  /* TODO: Handle audio
   * Create a SoundState enum type for Player and Ball physics objects
   * The physics objects will update their sound state whenever the sound is triggered
   * A method here will "scan" the players and ball objects to check if the sound is required
   * and play whatever using the mixer API.
   *
   * TODO 2: Think about how to handle the sound reset so it is not retriggered.
  */
  SDL_RenderPresent(sdl_sys_.get_renderer());
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

void Game::intro_state() {
  // Render the view and update frame counter
  intro_view_->render(frame_counter_);
  frame_counter_++;
  // Check if the state must change
  if (frame_counter_ >= view::IntroView::max_frames || menu_input_.enter) {
    // Setup stuff for next state
    state_ = GameState::Menu;
    menu_state_ = MenuState::Menu;
    player_selection_ = MenuPlayerSelection::SINGLE_PLAYER;
    frame_counter_ = 0;
    menu_view_->start();
  }
}

void Game::menu_state() {
  // Render the view and update frame counter
  menu_view_->render(frame_counter_);
  frame_counter_++;

  // Update menu state
  switch (menu_state_) {
  case MenuState::Menu:
    if (frame_counter_ < view::MenuView::start_frames && menu_input_.enter) {
      // The animation is skipped by pressing enter
      frame_counter_ = view::MenuView::start_frames;
      return;
    }
    // On the first frames, just render the animation (skip input processing).
    if (frame_counter_ <= view::MenuView::start_frames) {
      return;
    }

    // Process input to update the game mode selection
    if (player_selection_ == MenuPlayerSelection::SINGLE_PLAYER && menu_input_.down) {
      player_selection_ = MenuPlayerSelection::MULTI_PLAYER;
      menu_view_->change_selection(player_selection_);
      sdl_sys_.get_sound()->pi();
    }
    else if (player_selection_ == MenuPlayerSelection::MULTI_PLAYER && menu_input_.up) {
      player_selection_ = MenuPlayerSelection::SINGLE_PLAYER;
      menu_view_->change_selection(player_selection_);
      sdl_sys_.get_sound()->pi();
    }

    // Process input to check if the game must start
    if (menu_input_.enter) {
      menu_state_ = MenuState::FadeOut;
      menu_view_->set_state(menu_state_);
      sdl_sys_.get_sound()->pikachu();
    }
    break;
  case MenuState::FadeOut:
    // After fading out completely, change game state to start the game
    if (menu_view_->get_fade_alpha() >= 1.0) {
      // Trigger transition to VolleyGame state
      state_ = GameState::VolleyGame;
      frame_counter_ = 0;
      SDL_Log("Player selection: %d", player_selection_);
      // TODO: Change controllers based on game mode selection
      controller_left_->on_game_start(PhysicsView(*physics_));
      controller_right_->on_game_start(PhysicsView(*physics_));
      volley_view_->start();
    }
    break;
  }
}

void Game::volley_state() {
  // Render the view and update frame counter
  volley_view_->render(frame_counter_, PhysicsView(*physics_));
  frame_counter_++;

  switch (volley_state_) {
  case VolleyGameState::NewGame:
      // TODO: Maybe check transitions after rendering and updating
      if (frame_counter_ >= view::VolleyView::new_game_frames) {
        volley_state_ = VolleyGameState::PlayRound;
        volley_view_->set_state(volley_state_);
      }
    break;
    case VolleyGameState::StartRound:
      if (frame_counter_ >= view::VolleyView::start_round_frames) {
        // Start the next round
        // When a new round stars, update the controllers
        controller_left_->on_round_start(PhysicsView(*physics_));
        controller_right_->on_round_start(PhysicsView(*physics_));
        volley_state_ = VolleyGameState::PlayRound;
        volley_view_->set_state(volley_state_);
      }
    break;
    case VolleyGameState::PlayRound:
      // Update physics and check if the ball is touching the ground
      if (physics_->update(input_left_, input_right_)) {
        // End of the round
        next_serve_side_ = update_score();
        volley_view_->set_score(score_left_, score_right_);
        if (score_left_ >= win_score || score_right_ >= win_score) {
          // Game ended
          physics_->end_game(next_serve_side_);
          frame_counter_ = 0;
          volley_state_ = VolleyGameState::GameEnd;
          volley_view_->set_state(volley_state_);
        }
        else {
          // Apply end-of-round effects and start next round
          frame_counter_ = 0;
          volley_state_ = VolleyGameState::EndRound;
          volley_view_->set_state(volley_state_);
        }
      }
    break;
    case VolleyGameState::EndRound:
      // Apply and manage slow motion and fading effects
      // Slow motion will be active for the first 6 frames after the point
      slow_motion_ = frame_counter_ <= 6;
      // We keep updating the physics, but without checking the ball
      physics_->update(input_left_, input_right_);
      if (frame_counter_ >= view::VolleyView::end_round_frames) {
        // Start the next round
        frame_counter_ = 0;
        volley_view_->fade_out(1.0);
        physics_->init_round(next_serve_side_);
        volley_state_ = VolleyGameState::StartRound;
        volley_view_->set_state(volley_state_);
      }
    break;
    case VolleyGameState::GameEnd:
      // Check if the user wants to skip the end frames
      const bool skip =
        frame_counter_ > view::VolleyView::game_end_skip_frames && menu_input_.enter;
      // Or, end the game after the total animation frames
      if (skip || frame_counter_ > view::VolleyView::game_end_frames) {
        reset_volley_game_state();
        frame_counter_ = 0;
        state_ = GameState::Intro;
        intro_view_->start();
        return;
      }
      // Keep updating physics in the end state, without checking the ball touching ground.
      physics_->update(input_left_, input_right_);
    break;
  }
}

FieldSide Game::update_score() {
  if (physics_->ball().punch_effect_x() < ground_h_width) {
    score_right_++;
    return FieldSide::Right;
  }
  score_left_++;
  return FieldSide::Left;
}

void Game::reset_volley_game_state() {
  score_left_ = 0;
  score_right_ = 0;
  next_serve_side_ = FieldSide::Left;
  volley_state_ = VolleyGameState::NewGame;
  physics_->restart();
}

} // namespace pika
