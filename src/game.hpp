#ifndef PIKA_GAME_HPP
#define PIKA_GAME_HPP

#include "pikaball/input.hpp"
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
  std::unique_ptr<VolleyView> volley_view_ {nullptr};

  // Game state
  bool running_ {false};
  GameState state_ {GameState::Round};

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
