#include "game.hpp"
#include "SDL3/SDL.h"

#include <pikaball/controller/computer_controller.hpp>
#include <pikaball/controller/keyboard_controller.hpp>

namespace pika {

namespace keys {
/**
 * Default keyboard codes used for each action. Defined here to be easily
 * changed.
 * TODO: Future feature: Allow remapping of keys
 */

constexpr int p1_hit = SDL_SCANCODE_Z;
constexpr int p1_hit_alt = SDL_SCANCODE_SPACE;
constexpr int p2_hit = SDL_SCANCODE_RETURN;
constexpr int p2_hit_alt = SDL_SCANCODE_KP_ENTER;

constexpr int p1_left = SDL_SCANCODE_D;
constexpr int p1_right = SDL_SCANCODE_G;
constexpr int p1_up = SDL_SCANCODE_R;
constexpr int p1_down = SDL_SCANCODE_F;

constexpr int p2_left = SDL_SCANCODE_LEFT;
constexpr int p2_right = SDL_SCANCODE_RIGHT;
constexpr int p2_up = SDL_SCANCODE_UP;
constexpr int p2_down = SDL_SCANCODE_DOWN;

} // namespace pika::keys


Game::Game() {
  physics_ = std::make_unique<Physics>(),
  intro_view_ = std::make_unique<view::IntroView>(
    sdl_sys_.get_renderer(), sdl_sys_.get_sprite_sheet());
  menu_view_ = std::make_unique<view::MenuView>(
    sdl_sys_.get_renderer(), sdl_sys_.get_sprite_sheet());
  volley_view_ = std::make_unique<view::VolleyView>(
    sdl_sys_.get_renderer(), sdl_sys_.get_sprite_sheet());
  options_view_ = std::make_unique<view::OptionsView>(
    sdl_sys_.get_renderer(),
    sdl_sys_.get_sprite_sheet(),
    sdl_sys_.get_font()
  );
  // Initialize default option values
  options_view_->select_option(option_menu_select_);
  options_view_->select_speed(speed_opt_select_);
  options_view_->select_points(points_opt_select_);
  options_view_->select_music(music_opt_select_);

  // By default, both players are controlled by the keyboard
  controller_right_ = std::make_unique<KeyboardController>(FieldSide::Right);
  controller_left_ = std::make_unique<KeyboardController>(FieldSide::Left);
}

void Game::step() {
  // First, compile and process events
  compile_events();

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
    // TODO: If game is paused, controllers should not be queried
    input_left_ = controller_left_->on_update(PhysicsView(*physics_));
    input_right_ = controller_right_->on_update(PhysicsView(*physics_));
    volley_state();

    // Check physics state and play sounds accordingly
    handle_sound();

    // Check if the view needs slow motion and change the FPS
    const unsigned int fps = slow_motion_ ? slow_motion_fps_ : target_fps_;
    target_time_per_frame_ = ns_per_second / fps;
    break;
  }
  // Always enter the menu state. If the game is not paused it will return immediately.
  menu_options_state();

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
    // Get current input state from keyboard
    handle_input();
    // Execute game update step and draw
    step();
    const unsigned long end_time = SDL_GetTicksNS();

    const unsigned long sleep_time = target_time_per_frame_ - (end_time - start_time);
    SDL_DelayPrecise(sleep_time);
  }
}

void Game::handle_input() {
  // Event data
  SDL_Event event {};

  // Check and process all new events. Not thread safe.
  while (SDL_PollEvent(&event)) {
    handle_event(&event);
  }
}

void Game::handle_event(const SDL_Event * event) {
  if (event->type == SDL_EVENT_QUIT) {
    running_ = false;
  } else if (event->type == SDL_EVENT_KEY_DOWN && !event->key.repeat) {
    // Possibly meaningful event. Store it and process it later
    std::lock_guard lock(events_mutex_);
    events_queue_.push_back(*event);
  }
}

void Game::compile_events() {
  // Enter / power-hit keys are handled by events to avoid repetitions
  PlayerInput player_input_left {};
  PlayerInput player_input_right {};
  // Same applies to menu input
  menu_input_ = {};

  // Process all events in the queue
  {
    std::lock_guard lock(events_mutex_);
    for (const auto& event : events_queue_) {
      // Redundant check. Left in case more event types are added in the future (e.g. joystick)
      if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
        switch (event.key.scancode) {
        case keys::p1_hit:
        case keys::p1_hit_alt:
            player_input_left.power_hit = true;
            menu_input_.enter_left = true;
            break;
          case keys::p2_hit:
          case keys::p2_hit_alt:
          player_input_right.power_hit = true;
            menu_input_.enter_right = true;
            break;
          case SDL_SCANCODE_ESCAPE:
            // When ESC is pressed, the game is paused / unpaused
            if (state_ != GameState::Intro) {
              pause_ = !pause_;
            }
            break;
          case keys::p1_up:
          case keys::p2_up:
            menu_input_.up = true;
            break;
          case keys::p1_down:
          case keys::p2_down:
            menu_input_.down = true;
            break;
          case keys::p1_left:
          case keys::p2_left:
            menu_input_.left = true;
            break;
          case keys::p1_right:
          case keys::p2_right:
            menu_input_.right = true;
            break;
        default:
            break;
        }
      }
    }
    // Clear the event queue for the next update
    events_queue_.clear();
  }

  // Forget about events and just grab a snapshot of the current keyboard state
  const bool* key_state = SDL_GetKeyboardState(nullptr);

  // Convert keys from bool to a DirX/DirY object and set the player input directions
  player_input_left.direction_x =
    get_input_direction_x(key_state[keys::p1_left], key_state[keys::p1_right]);
  player_input_left.direction_y =
    get_input_direction_y(key_state[keys::p1_up], key_state[keys::p1_down]);
  player_input_right.direction_x =
    get_input_direction_x(key_state[keys::p2_left], key_state[keys::p2_right]);
  player_input_right.direction_y =
    get_input_direction_y(key_state[keys::p2_up], key_state[keys::p2_down]);

  // Pass keyboard inputs to the keyboard controllers (if controllers are keyboards)
  if (const auto kb_left = dynamic_cast<KeyboardController*>(controller_left_.get())) {
    kb_left->set_input(player_input_left);
  }
  if (const auto kb_right = dynamic_cast<KeyboardController*>(controller_right_.get())) {
    kb_right->set_input(player_input_right);
  }

  menu_input_.enter = menu_input_.enter_left | menu_input_.enter_right;
}

void Game::handle_player_sound(const FieldSide side) const {
  switch (physics_->player(side).sound()) {
  case PlayerSound::Pika:
    sdl_sys_.get_sound()->pika(side);
    break;
  case PlayerSound::Chu:
    sdl_sys_.get_sound()->chu(side);
    break;
  case PlayerSound::Pipikachu:
    sdl_sys_.get_sound()->pipikachu();
    break;
  default:
    break;
  }
}

void Game::handle_sound() const {
  // Handle Player sounds
  handle_player_sound(FieldSide::Left);
  handle_player_sound(FieldSide::Right);
  // Handle Ball sound
  switch (physics_->ball().sound()) {
  case BallSound::Hit:
    sdl_sys_.get_sound()->ball_hit();
    break;
  case BallSound::Ground:
    sdl_sys_.get_sound()->ball_ground();
    break;
  default:
    break;
  }
  physics_->reset_sound();
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
    player_selection_ = MenuPlayerSelection::SinglePlayer;
    frame_counter_ = 0;
    menu_view_->start();
  }
}

void Game::menu_state() {
  // Render the view
  menu_view_->render(frame_counter_);

  // If the game is paused (options are on the screen) just render and exit without an update
  if (pause_) {
    return;
  }

  // Update the frame counter
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
    if (player_selection_ == MenuPlayerSelection::SinglePlayer && menu_input_.down) {
      player_selection_ = MenuPlayerSelection::MultiPlayer;
      menu_view_->change_selection(player_selection_);
      sdl_sys_.get_sound()->pi();
    }
    else if (player_selection_ == MenuPlayerSelection::MultiPlayer && menu_input_.up) {
      player_selection_ = MenuPlayerSelection::SinglePlayer;
      menu_view_->change_selection(player_selection_);
      sdl_sys_.get_sound()->pi();
    }

    // Process input to check if the game must start
    if (menu_input_.enter) {
      // Create the proper player controllers according to menu selection
      if (player_selection_ == MenuPlayerSelection::MultiPlayer) {
        controller_left_ = std::make_unique<KeyboardController>(FieldSide::Left);
        controller_right_ = std::make_unique<KeyboardController>(FieldSide::Right);
      } else if (player_selection_ == MenuPlayerSelection::SinglePlayer) {
        if (menu_input_.enter_left) {
          controller_left_ = std::make_unique<KeyboardController>(FieldSide::Left);
          controller_right_ = std::make_unique<ComputerController>(FieldSide::Right);
        } else if (menu_input_.enter_right) {
          controller_left_ = std::make_unique<ComputerController>(FieldSide::Left);
          controller_right_ = std::make_unique<KeyboardController>(FieldSide::Right);
        } else {
          // This should never happen!!!!
          controller_left_ = std::make_unique<ComputerController>(FieldSide::Left);
          controller_right_ = std::make_unique<ComputerController>(FieldSide::Right);
        }
      }
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
      // Initialize controllers
      controller_left_->on_game_start(PhysicsView(*physics_));
      controller_right_->on_game_start(PhysicsView(*physics_));
      volley_view_->start();
      // Start the music (if enabled)
      if (music_opt_select_ == OnOffSelection::On) {
        sdl_sys_.get_sound()->start_music();
      }
    }
    break;
  }
}

void Game::menu_options_state() {
  if (!pause_ || state_ == GameState::Intro) {
    return;
  }

  switch (option_menu_select_) {
  case OptionMenuSelection::Speed:
    if (menu_input_.down) {
      option_menu_select_ = OptionMenuSelection::Points;
    } else if (menu_input_.up) {
      option_menu_select_ = OptionMenuSelection::Music;
    }
    // Change option values if left/right
    switch (speed_opt_select_) {
    case SpeedOptionSelection::Slow:
      if (menu_input_.right) {
        change_game_speed(SpeedOptionSelection::Medium);
      }
      break;
    case SpeedOptionSelection::Medium:
      if (menu_input_.right) {
        change_game_speed(SpeedOptionSelection::Fast);
      } else if (menu_input_.left) {
        change_game_speed(SpeedOptionSelection::Slow);
      }
      break;
    case SpeedOptionSelection::Fast:
      if (menu_input_.left) {
        change_game_speed(SpeedOptionSelection::Medium);
      }
      break;
    }
    break;
  case OptionMenuSelection::Points:
    if (menu_input_.down) {
      option_menu_select_ = OptionMenuSelection::Music;
    } else if (menu_input_.up) {
      option_menu_select_ = OptionMenuSelection::Speed;
    }
    // Change option values if left/right
    switch (points_opt_select_) {
    case PointsOptionSelection::Five:
      if (menu_input_.right) {
        change_win_score(PointsOptionSelection::Ten);
      }
      break;
    case PointsOptionSelection::Ten:
      if (menu_input_.right) {
        change_win_score(PointsOptionSelection::Fifteen);
      } else if (menu_input_.left) {
        change_win_score(PointsOptionSelection::Five);
      }
      break;
    case PointsOptionSelection::Fifteen:
      if (menu_input_.left) {
        change_win_score(PointsOptionSelection::Ten);
      }
      break;
    }
    break;
  case OptionMenuSelection::Music:
    if (menu_input_.down) {
      option_menu_select_ = OptionMenuSelection::Speed;
    } else if (menu_input_.up) {
      option_menu_select_ = OptionMenuSelection::Points;
    }
    // Change option values if left/right
    if (music_opt_select_ == OnOffSelection::On && menu_input_.right) {
      toggle_music();
      music_opt_select_ = OnOffSelection::Off;
    } else if (music_opt_select_ == OnOffSelection::Off && menu_input_.left) {
      toggle_music();
      music_opt_select_ = OnOffSelection::On;
    }
    break;
  }

  options_view_->select_option(option_menu_select_);
  options_view_->select_speed(speed_opt_select_);
  options_view_->select_points(points_opt_select_);
  options_view_->select_music(music_opt_select_);

  options_view_->render();
}

void Game::volley_state() {
  // Render the view
  volley_view_->render(frame_counter_, PhysicsView(*physics_));

  // If the game is paused (options are on the screen) just render and exit without an update
  if (pause_) {
    return;
  }

  // Update frame counter
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
          // Stop music
          sdl_sys_.get_sound()->stop_music();
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

void Game::change_game_speed(const SpeedOptionSelection speed) {
  speed_opt_select_ = speed;
  switch (speed) {
    case SpeedOptionSelection::Slow:
      target_fps_ = 20;
      break;
    case SpeedOptionSelection::Medium:
      target_fps_ = 25;
      break;
    case SpeedOptionSelection::Fast:
      target_fps_ = 30;
      break;
  }
  target_time_per_frame_ = ns_per_second / target_fps_;
}
void Game::change_win_score(const PointsOptionSelection win_points) {
  points_opt_select_ = win_points;
  switch (win_points) {
  case PointsOptionSelection::Five:
    win_score = 5;
    break;
  case PointsOptionSelection::Ten:
    win_score = 10;
    break;
  case PointsOptionSelection::Fifteen:
    win_score = 15;
    break;
  }
}

void Game::toggle_music() {
  if (music_opt_select_ == OnOffSelection::Off) {
    // Only start the music if inside a game
    if (state_ == GameState::VolleyGame) {
      sdl_sys_.get_sound()->start_music();
    }
    music_opt_select_ = OnOffSelection::On;
  } else {
    sdl_sys_.get_sound()->stop_music();
    music_opt_select_ = OnOffSelection::Off;
  }
}

void Game::reset_volley_game_state() {
  score_left_ = 0;
  score_right_ = 0;
  next_serve_side_ = FieldSide::Left;
  volley_state_ = VolleyGameState::NewGame;
  physics_->restart();
}

} // namespace pika
