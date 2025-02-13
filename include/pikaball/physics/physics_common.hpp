/**
* This file contains common constants and parameters used by the physics engine
*/
#ifndef PIKA_PHYSICS_COMMON_HPP
#define PIKA_PHYSICS_COMMON_HPP
#include "pikaball/common.hpp"

namespace pika {

/** Ground width */
constexpr unsigned int ground_width = screen_width;
/** Ground half-width, also the x coordinate for the net pillar */
constexpr unsigned int ground_h_width = (ground_width / 2);
/** Player (Pikachu) size: width = height = 64 */
constexpr unsigned int player_size = 64;
/** Player half size */
constexpr unsigned int player_h_size = (player_size / 2); // integer division
/** Player's y coordinate when they are touching ground */
constexpr unsigned int player_ground_y = 244;
/** Ball's radius */
constexpr unsigned int ball_radius = 20;
/** Ball's y coordinate when it is touching ground */
constexpr unsigned int ball_ground_y = 252;
/** Net pillar's half width */
constexpr unsigned int net_pillar_h_width = 25;
/** Net pillar top's top side y coordinate */
constexpr unsigned int net_top_top_y = 176;
/** Net pillar top's bottom side y coordinate */
constexpr unsigned int net_top_bottom_y = 192;

/**
 * limit the looping number of the infinite loops.
 * This constant is not in the original machine code.
 * (The original machine code does not limit the looping number.)
 *
 * In the original, ball x coord range setting (ball x coord in [20, 432]),
 * the infinite loops in calculateExpectedLandingPointXFor function and
 * expectedLandingPointXWhenPowerHit function seems to be always terminated soon.
 * But if the ball x coord range is edited, for example, to [20, 432 - 20] for left-right symmetry,
 * it is observed that the infinite loop in expectedLandingPointXWhenPowerHit does not terminate.
 * So for safety, this infinite loop limit is included for the infinite loops mentioned above.
 */
constexpr unsigned int infinite_loop_limit = 1000;

/**
 * Enum that represents the side (left/right) of the field
 * Used to initialize the ball and the players.
 */
enum class FieldSide {
  Left,
  Right,
};

} // namespace pika

#endif // PIKA_PHYSICS_COMMON_HPP
