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

enum class PlayerSound {
  None,
  Pika,
  Chu,
  Pipikachu
};

/**
 * Class representing a Pikachu player.
 *
 * Player 1 property address: 00411F28 -> +28 -> +10 -> +C -> ...
 * Player 2 property address: 00411F28 -> +28 -> +10 -> +10 -> ...
 * The "..." part is written on the line comment on the right side of each property.
 * e.g. address to player1.isPlayer: 00411F28 -> +28 -> +10 -> +C -> +A0
 * e.g. address to player2.isComputer: 00411F28 -> +28 -> +10 -> +10 -> +A4
 *
 * For initial values: refer to FUN_000403a90 && FUN_00401f40
 */
class Player {
public:
  explicit Player(const FieldSide& field_side);
  ~Player() = default;
  Player(const Player&) = default;
  Player& operator=(const Player&) = default;

  // Delete move operations
  Player(Player&&) = delete;
  Player& operator=(Player&&) = delete;

  /** Initialize the player for a new game (reset state) */
  void initialize_game();

  /**
   * Initialize the player for a new round (reset state)
   */
  void initialize_round();

  /**
   * Update player state according to user input.
   * FUN_00401fc0
   * @param input The player's input
   */
  void update(const PlayerInput& input);

  void end_game(bool is_winner);

  /** Resets the current sound state to avoid re-triggers */
  void reset_sound();

  // Getters
  [[nodiscard]] auto x() const { return x_; }
  [[nodiscard]] auto y() const { return y_; }
  [[nodiscard]] auto state() const { return state_; }
  [[nodiscard]] auto side() const { return field_side_; }
  [[nodiscard]] auto diving_direction() const { return diving_direction_; }
  [[nodiscard]] auto anim_frame_number() const { return anim_frame_number_; }
  [[nodiscard]] auto sound() const { return sound_; }

  // Flag to remember if the ball was already touched
  bool collision_with_ball {false};  // 0xBC

private:
  // Player coordinates
  int x_ {36};               // 0xA8, initialized to 36 (left) or 396 (right)
  int y_ {player_ground_y};  // 0xAC
  // y velocity is positive when going down (gravity)
  int velocity_y_ {0};                // 0xB0

  // Current animation frame number
  int anim_frame_number_ {0};  // 0xC4
  // Arm swinging direction in the normal mode animation
  int anim_arm_direction_ {1};  // 0xC8
  // Delay before switching to the next frame in the animation sequence.
  int anim_frame_delay_ {0};    // 0xCC

  // Remaining time for the player to lay on the ground after diving
  int lying_down_timer_ = {-1};  // 0xB8

  // The side of the volley field where this player plays
  FieldSide field_side_ {FieldSide::Left};

  PlayerState state_ {PlayerState::Normal};  // 0xC0 (the state is an integer in the OG game).
  // Current sound state for this player
  PlayerSound sound_ {PlayerSound::None};
  // Diving direction. Possible values: -1 (left), 0 (no diving), 1 (right)
  DirX diving_direction_ {DirX::None};     // 0xB4

  bool is_winner_ {false};    // 0xD0
  bool game_ended_ {false};   // 0xD0
};

} // namespace pika

#endif // PIKA_PLAYER_HPP
