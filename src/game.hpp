#ifndef PIKA_GAME_HPP
#define PIKA_GAME_HPP

#include "pikaball/game_state.hpp"
#include "pikaball/input.hpp"
#include "view/intro_view.hpp"
#include "view/menu_view.hpp"
#include "view/volley_view.hpp"
#include "window.hpp"

#include <pikaball/controller/player_controller.hpp>
#include <pikaball/physics/physics.hpp>

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

  // Main (and only) physics object to update the state of ball and players
  Physics::Ptr physics_ {nullptr};

  // Views
  std::unique_ptr<view::IntroView> intro_view_ {nullptr};
  std::unique_ptr<view::MenuView> menu_view_ {nullptr};
  std::unique_ptr<view::VolleyView> volley_view_ {nullptr};

  // Frame rate management
  unsigned int target_fps_ {25}; // Possible speeds are 20 / 25 / 30 fps
  // Speed for slow motion effect (fixed to 5 FPS)
  constexpr static unsigned int slow_motion_fps_ {5};
  // Time in nanoseconds per frame. This variable will be changed when slow  motion is applied
  unsigned long target_time_per_frame_ {ns_per_second / target_fps_};

  // Game state
  bool running_ {false};
  GameState state_ {GameState::Intro};

  // Inputs
  MenuInput menu_input_ {};
  // Player controllers
  std::unique_ptr<PlayerController> controller_left_ {nullptr};
  std::unique_ptr<PlayerController> controller_right_ {nullptr};

  /**
  * Handle keyboard input and update input structs
  */
  void handle_input();
};

} // namespace pika

#endif // PIKA_GAME_HPP
