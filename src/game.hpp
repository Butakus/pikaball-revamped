#ifndef PIKA_GAME_HPP
#define PIKA_GAME_HPP

#include "window.hpp"

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
};

} // namespace pika

#endif // PIKA_GAME_HPP
