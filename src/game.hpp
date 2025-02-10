#ifndef PIKA_GAME_HPP
#define PIKA_GAME_HPP

#include "pikaball/input.hpp"
#include "pikaball/game_state.hpp"
#include "view/intro_view.hpp"
#include "view/menu_view.hpp"
#include "view/volley_view.hpp"
#include "window.hpp"

namespace pika {

class Game {
public:
  Game();
  ~Game() = default;

  Game(Game const&) = delete;
  Game(Game &&) = delete;
  Game &operator=(Game const&) = delete;
  Game &operator=(Game &&) = delete;

  /**
   * Game loop iteration. Handle input, control game state, and updates views.
   */
  void step();

  /**
   * Run the game continuously at the selected frame rate.
   * This method does not return until the window is closed.
   */
  void run();

private:
  Window window_;
  // Views
  std::unique_ptr<view::IntroView> intro_view_ {nullptr};
  std::unique_ptr<view::MenuView> menu_view_ {nullptr};
  std::unique_ptr<view::VolleyView> volley_view_ {nullptr};

  // Frame rate management
  Uint64 target_fps_ {25}; // Possible speeds are 20 / 25 / 30 fps
  // Time in nanoseconds per frame.
  Uint64 target_time_per_frame_ {SDL_NS_PER_SECOND / target_fps_};

  // Game state
  bool running_ {false};
  GameState state_ {GameState::Intro};

  // Inputs
  PlayerInput player_input_left_ {};
  PlayerInput player_input_right_ {};
  MenuInput menu_input_ {};

  /**
  * Handle keyboard input and update input structs
  */
  void handle_input();
};

} // namespace pika

#endif // PIKA_GAME_HPP
