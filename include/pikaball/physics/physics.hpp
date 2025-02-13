#ifndef PIKA_PHYSICS_HPP
#define PIKA_PHYSICS_HPP

#include "player.hpp"
#include "ball.hpp"
#include "pikaball/input.hpp"

namespace pika {

class Physics {
public:
  Physics();
  Physics(bool is_computer_left, bool is_computer_right);
  ~Physics() = default;

  // Delete copy and move operations
  Physics(const Physics&) = delete;
  Physics& operator=(const Physics&) = delete;
  Physics(Physics&&) = delete;
  Physics& operator=(Physics&&) = delete;

  /**
   * Update the world state based on the user input.
   * If any of the players is controlled by the computer, its input is ignored
   * and its commands are computed by function TODO: XXXX.
   * FUN_00403dd0 / physicsEngine
   * @param input_left Input for the left player.
   * @param input_right Input for the right player.
   * @return True if the ball touches the ground after the update.
   */
  bool update(const PlayerInput& input_left, const PlayerInput& input_right);

private:
  Player player_left_;
  Player player_right_;
  Ball ball_;

  /**
   * Check and process collisions between the ball and a player
   * @param player a reference to one of the players
   * @param input the input for the given player
   */
  void collision_ball_player(Player& player, const PlayerInput& input);

  /**
   * Update the position of a player
   * @param player a reference to one of the players
   * @param input the input for the given player
   */
  static void update_player(Player& player, const PlayerInput& input);

};

} // namespace pika

#endif // PIKA_PHYSICS_HPP
