#ifndef PIKA_PLAYER_HPP
#define PIKA_PLAYER_HPP

#include "physics_common.hpp"

#include <pikaball/input.hpp>

namespace pika {

enum class PlayerState {
  Normal,
  Jumping,
  PowerHit,  // Jumping and power-hitting
  Diving,
  AfterDiving,  // Lay down on the ground after a dive
  Winner,
  Loser
};

/**
 * Class representing a Pikachu player
 *
 *  1 property address: 00411F28 -> +28 -> +10 -> +C -> ...
 * Player 2 property address: 00411F28 -> +28 -> +10 -> +10 -> ...
 * The "..." part is written on the line comment on the right side of each property.
 * e.g. address to player1.isPlayer: 00411F28 -> +28 -> +10 -> +C -> +A0
 * e.g. address to player2.isComputer: 00411F28 -> +28 -> +10 -> +10 -> +A4
 *
 * For initial values: refer to FUN_000403a90 && FUN_00401f40
 */
class Player {
public:
  Player(const FieldSide& field_side, bool is_computer);

  // Delete move operations
  Player(Player&&) = delete;
  Player& operator=(Player&&) = delete;

  /**
   * Initialize the ball for a new game (reset state)
   */
  void initialize_game();

  /**
   * Initialize the ball for a new round (reset state)
   */
  void initialize_round();

  /**
   * Update player state according to user input.
   * FUN_00401fc0
   * @param input The player's input
   */
  void update(const PlayerInput& input);

  void end_game(bool is_winner);

  // Getters
  [[nodiscard]] const auto& x() const { return x_; }
  [[nodiscard]] const auto& y() const { return y_; }
  [[nodiscard]] const auto& state() const { return state_; }
  [[nodiscard]] const auto& side() const { return field_side_; }
  [[nodiscard]] const auto& diving_direction() const { return diving_direction_; }
  [[nodiscard]] const auto& anim_frame_number() const { return anim_frame_number_; }
  [[nodiscard]] const auto& is_computer() const { return is_computer_; }

  // Flag to remember if the ball was already touched
  bool collision_with_ball {false};  // 0xBC

private:
  // Player coordinates
  unsigned int x_ {36};               // 0xA8, initialized to 36 (left) or 396 (right)
  unsigned int y_ {player_ground_y};  // 0xAC
  // y velocity is positive when going down (gravity)
  int velocity_y_ {0};                // 0xB0
  PlayerState state_ {PlayerState::Normal};  // 0xC0 (the state is an integer in the OG game).
  // Current animation frame number
  unsigned int anim_frame_number_ {0};  // 0xC4
  // Arm swinging direction in the normal mode animation
  int anim_arm_direction_ {1};  // 0xC8
  // Delay before switching to the next frame in the animation sequence.
  int anim_frame_delay_ {0};    // 0xCC

  // The side of the volley field where this player plays
  FieldSide field_side_ {FieldSide::Left};

  // Diving direction. Possible values: -1 (left), 0 (no diving), 1 (right)
  int diving_direction_ {0};     // 0xB4
  // Remaining time for the player to lay on the ground after diving
  int lying_down_timer_ = {-1};  // 0xB8

  bool is_winner_ {false};    // 0xD0
  bool game_ended_ {false};   // 0xD0

  bool is_computer_ {false};  // 0xA4
  /**
   * It flips randomly to 0 or 1 by the letComputerDecideUserInput function (FUN_00402360)
   * when ball is hanging around on the other player's side.
   * If it is 0, computer player stands by around the middle point of their side.
   * If it is 1, computer player stands by adjacent to the net.
   */
  int computer_idle_position_ {0};  // 0xDC
  /**
   * This value is initialized to (_rand() % 5) before the start of every round.
   * The greater the number, the bolder the computer player.
   *
   * If computer has higher boldness,
   * judges more the ball is hanging around the other player's side,
   * has greater distance to the expected landing point of the ball,
   * jumps more, and dives less.
   * See the source code of the letComputerDecideUserInput function (FUN_00402360).
   *
   * Possible values: 0, 1, 2, 3 or 4
   */
  int computer_boldness_ {0};  // 0xD8
};

} // namespace pika

#endif // PIKA_PLAYER_HPP
