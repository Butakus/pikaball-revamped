#ifndef PIKA_GAME_HPP
#define PIKA_GAME_HPP

#include "window.hpp"
#include "input.hpp"

namespace pika {

class Game {
public:
  Game() = default;

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
  bool running_ {false};
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
