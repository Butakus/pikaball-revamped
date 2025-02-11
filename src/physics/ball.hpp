#ifndef PIKA_BALL_HPP
#define PIKA_BALL_HPP

#include <array>

#include <pikaball/input.hpp>
#include "physics_common.hpp"
#include "player.hpp"

namespace pika {

class Ball {
public:
  Ball() = default;
  ~Ball() = default;
  Ball(const Ball&) = default;
  Ball& operator=(const Ball&) = default;

  // Delete move operations
  Ball(Ball&&) = delete;
  Ball& operator=(Ball&&) = delete;

  /**
   * Initialize the ball for a new round (reset state)
   * @param field_side the side of the field where the ball will be placed.
   */
  void initialize(const FieldSide& field_side);

  /**
   * Process collision between the ball and the world and update the ball state
   * FUN_00402dc0 / processCollisionBetweenBallAndWorldAndSetBallPosition()
   * @return true if the ball is touching the ground
   */
  bool update();

  /**
   * Calculate the x coordinate of the landing point.
   * This function simulates the future ball assuming no players until reaching
   * the ground. FUN_004031b0
   */
  void calculate_landing_point();

  /**
   * Check if there is a collision between the ball and a player
   * FUN_00403070
   * @param player a reference to one of the players
   * @return true if the ball is touching the given player
   */
  [[nodiscard]] bool collision_with_player(const Player& player) const;

  /**
   * Process the collision between the ball and a player.
   * The player may be on the ground or jumping and power-hitting.
   * This function only sets velocity and expected landing point of the ball.
   * FUN_004030a0 / processCollisionBetweenBallAndPlayer
   * @param player The player that is in contact with the ball.
   * @param input The input for the given player.
   */
  void process_player_hit(const Player& player, const PlayerInput& input);

  // Getters
  [[nodiscard]] const auto& x() const { return x_; }
  [[nodiscard]] const auto& y() const { return y_; }
  [[nodiscard]] const auto& velocity_x() const { return velocity_x_; }
  [[nodiscard]] const auto& velocity_y() const { return velocity_y_; }
  [[nodiscard]] const auto& rotation() const { return rotation_; }

private:
  // Ball coordinates and velocities
  unsigned int x_ {56};  // 0x30, initialized to 56 (left) or 376 (right)
  unsigned int y_ {0};   // 0x34
  int velocity_x_ {0};   // 0x38
  // y velocity is positive when going down (gravity)
  int velocity_y_ {1};   // 0x3C
  /**
   * Ball rotation frame number selector (animation).
   * During the period where it continues to be 5, hyper ball glitch occur.
   * Possible values: 0, 1, 2, 3, 4 or 5
   */
  unsigned int rotation_ {0};             // 0x44
  unsigned int fine_rotation_ {0};        // 0x48
  // Coordinates and size of the punch effect
  unsigned int punch_effect_x_ {0};       // 0x50
  unsigned int punch_effect_y_ {0};       // 0x54
  unsigned int punch_effect_radius_ {0};  // 0x4C
  bool power_hit_ {false};                // 0x68
  unsigned int expected_landing_x_ {0};   // 0x40

  // Previous ball coordinates for trailing effect for power hit
  std::array<unsigned int, 2> trailing_x_ {0};  // 0x58, 0x5C
  std::array<unsigned int, 2> trailing_y_ {0};  // 0x60, 0x64


  /**
   * Part of the update() function (FUN_00402dc0) that only
   * checks collisions and updates the ball state (no rotation or trailing).
   * This code has been isolated to be reused in calculate_landing_point()
   * @return true if the ball is touching the ground
   */
  bool update_position();
};

} // namespace pika

#endif // PIKA_BALL_HPP
