#ifndef PIKA_PHYSICS_HPP
#define PIKA_PHYSICS_HPP

#include "ball.hpp"
#include "pikaball/input.hpp"
#include "pikaball/random.hpp"
#include "player.hpp"

#include <cstdint>
#include <memory>
#include <random>

namespace pika {

class Physics {
public:
  // Utility typedef
  using Ptr = std::unique_ptr<Physics>;

  /**
   * @param seed Seed for the simulation's random numbers.
   *             Networked games must use the same seed on both machines.
   */
  explicit Physics(std::uint32_t seed = std::random_device{}());
  ~Physics() = default;

  // Delete copy and move operations
  Physics(const Physics&) = delete;
  Physics& operator=(const Physics&) = delete;
  Physics(Physics&&) = delete;
  Physics& operator=(Physics&&) = delete;

  /**
   * Reset ball and players positions for the new round
   * @param field_side The side to serve in this round
   */
  void init_round(const FieldSide& field_side);

  /** Reset ball and players positions for a new game */
  void restart();

  /**
   * Update the world state based on the players' input.
   * FUN_00403dd0 / physicsEngine
   * @param input_left Input for the left player.
   * @param input_right Input for the right player.
   * @return True if the ball touches the ground after the update.
   */
  bool update(const PlayerInput& input_left, const PlayerInput& input_right);

  /**
   * Update the players' state to show the winner / loser animation
   * @param field_side The side who won the game
   */
  void end_game(const FieldSide& field_side);

  /** Reset sounds of players and ball after an iteration */
  void reset_sound();

  /** Restart the sequence of the random numbers with a new seed */
  void seed(const std::uint32_t seed) { rng_.seed(seed); }

  [[nodiscard]] const Ball& ball() const { return ball_; }
  [[nodiscard]] const Player& player(const FieldSide& side) const;
private:
  Player player_left_;
  Player player_right_;
  Ball ball_ {};
  // Random numbers of the simulation
  Rng rng_;

  /**
   * Check and process collisions between the ball and a player
   * @param player a reference to one of the players
   * @param input the input for the given player
   */
  void collision_ball_player(Player& player, const PlayerInput& input);

};

/**
 * A simple interface to the Physics object to be used by the controllers.
 * This prevents the Controller from casting const away
 * from the Physics object and doing nasty stuff
 *
 * The Object holds a const copy to the Ball and Player objects.
 */
class PhysicsView {
public:
  explicit PhysicsView(const Physics& physics) :
    ball(physics.ball()),
    player_left(physics.player(FieldSide::Left)),
    player_right(physics.player(FieldSide::Right))
  {}
  ~PhysicsView() = default;

  /** Two views are equal if the ball and both players are equal */
  bool operator==(const PhysicsView&) const = default;

  const Ball ball;
  const Player player_left;
  const Player player_right;
};

} // namespace pika

#endif // PIKA_PHYSICS_HPP
