#ifndef PIKA_GAME_HPP
#define PIKA_GAME_HPP

#include "pikaball/input.hpp"
#include "view/intro_view.hpp"
#include "view/menu_view.hpp"
#include "view/volley_view.hpp"
#include "window.hpp"

namespace pika {

// TODO: Combine states inside each View (with its own FSM)
enum class GameState {
  Intro,
  Menu,
  AfterMenuSelection,
  BeforeNewGame,
  StartNewGame,
  Round,
  AfterRoundEnd,
  BeforeNextRound
};

class Game {
public:
  Game();
  ~Game() = default;

  Game(Game const&) = delete;
  Game(Game &&) = delete;
  Game &operator=(Game const&) = delete;
  Game &operator=(Game &&) = delete;

  /**
   * Main loop. Handle input, control game state, and updates views.
   */
  void run();

private:
  Window window_;
  // Views
  std::unique_ptr<view::IntroView> intro_view_ {nullptr};
  std::unique_ptr<view::MenuView> menu_view_ {nullptr};
  std::unique_ptr<view::VolleyView> volley_view_ {nullptr};

  // Frame rate management
  Uint64 last_render_time_ {0};
  Uint64 target_fps_ {25}; // Possible speeds are 20 / 25 / 30 fps
  Uint64 target_time_per_frame_ {1000 / target_fps_};

  // Game state
  bool running_ {false};
  GameState state_ {GameState::Intro};
  unsigned int frame_counter_ {0};

  // Inputs
  PlayerInput player_input_1_ {};
  PlayerInput player_input_2_ {};
  MenuInput menu_input_ {};

  /**
  * Handle keyboard input and update input structs
  */
  void handle_input();
};

} // namespace pika

#endif // PIKA_GAME_HPP
