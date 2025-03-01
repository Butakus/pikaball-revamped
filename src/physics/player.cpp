#include <algorithm>
#include <pikaball/physics/player.hpp>
#include <pikaball/random.hpp>

namespace pika {
Player::Player(const FieldSide &field_side) {
  field_side_ = field_side;
  initialize_game();
}

void Player::initialize_game() {
  game_ended_ = false;
  is_winner_ = false;
  initialize_round();
}

void Player::initialize_round() {
  x_ = (field_side_ == FieldSide::Left) ? 36 : ground_width - 36;
  y_ = player_ground_y;
  velocity_y_ = 0;

  state_ = PlayerState::Normal;

  anim_frame_number_ = 0;
  anim_arm_direction_ = 1;
  anim_frame_delay_ = 0;

  // TODO: This must be initialized at the ComputerController class
  if (is_computer_) {
    computer_boldness_ = rand_int() % 5;
  }
}

void Player::update(const PlayerInput& input) {
  // Convert the left/right input keys to a [-1, 0, 1] integer
  // const int input_direction_x = get_input_direction_x(input);

  // if player is lying down... don't move
  if (state_ == PlayerState::AfterDiving) {
    lying_down_timer_--;
    if (lying_down_timer_ < -1) {
      state_ = PlayerState::Normal;
    }
    return;
  }

  // Process x-direction movement
  int velocity_x = 0;
  switch (state_) {
  case PlayerState::Normal:
  case PlayerState::Jumping:
  case PlayerState::PowerHit:
    velocity_x = static_cast<int>(input.direction_x) * 6;
    break;
  case PlayerState::Diving:
  case PlayerState::AfterDiving:
    velocity_x = static_cast<int>(diving_direction_) * 8;
    break;
  default:
    break;
  }

  const unsigned int next_x = x_ + velocity_x;
  x_ = next_x;

  // Limit player position to world boundaries
  if (field_side_ == FieldSide::Left) {
    x_ = std::clamp(next_x, player_h_size, ground_h_width - player_h_size);
  } else {
    x_ = std::clamp(next_x, ground_h_width + player_h_size, ground_width - player_h_size);
  }


  // Jump!
  if (state_ == PlayerState::Normal &&
      input.direction_y == DirY::Up &&
      y_ == player_ground_y) {
    velocity_y_ = -16;
    state_ = PlayerState::Jumping;
    anim_frame_number_ = 0;

    // TODO: Sound
    // maybe-stereo-sound function FUN_00408470 (0x90) omitted:
    // refer to a detailed comment above about this function
    // maybe-sound code function (playerpointer + 0x90 + 0x10)? omitted
    // player.sound.chu = true;
  }

  // Gravity
  const unsigned int next_y = y_ + velocity_y_;
  y_ = next_y;
  if (next_y < player_ground_y) {
    velocity_y_ += 1;
  } else if (next_y > player_ground_y) {
    // if player is landing...
    velocity_y_ = 0;
    y_ = player_ground_y;
    anim_frame_number_ = 0;
    if (state_ == PlayerState::Diving) {
      // if player is diving...
      state_ = PlayerState::AfterDiving;
      anim_frame_number_ = 0;
      lying_down_timer_ = 3;
    } else {
      state_ = PlayerState::Normal;
    }
  }

  if (input.power_hit) {
    if (state_ == PlayerState::Jumping) {
      // If player is jumping... POWER HIT!!
      anim_frame_delay_ = 5;
      anim_frame_number_ = 0;
      state_ = PlayerState::PowerHit;

      // TODO: Sound
      // maybe-sound function (playerpointer + 0x90 + 0x18)? omitted
      // maybe-stereo-sound function FUN_00408470 (0x90) omitted:
      // refer to a detailed comment above about this function
      // maybe-sound function (playerpointer + 0x90 + 0x14)? omitted
      // player.sound.pika = true;
    }
    else if (state_ == PlayerState::Normal && input.direction_x != DirX::None) {
      // Diving!!
      state_ = PlayerState::Diving;
      anim_frame_number_ = 0;
      diving_direction_ = input.direction_x;
      velocity_y_ = -5;

      // TODO: Sound
      // maybe-stereo-sound function FUN_00408470 (0x90) omitted:
      // refer to a detailed comment above about this function
      // maybe-sound code function (playerpointer + 0x90 + 0x10)? omitted
      // player.sound.chu = true;
    }
  }

  // Animations
  if (state_ == PlayerState::Jumping) {
  anim_frame_number_ = (anim_frame_number_ + 1) % 3;
  }
  else if (state_ == PlayerState::PowerHit) {
    if (anim_frame_delay_ < 1) {
      anim_frame_number_++;
      if (anim_frame_number_ > 4) {
        anim_frame_number_ = 0;
        state_ = PlayerState::Jumping;
      }
    }
    else {
      anim_frame_delay_--;
    }
  }
  else if (state_ == PlayerState::Normal) {
    anim_frame_delay_++;
    if (anim_frame_delay_ > 3) {
      anim_frame_delay_ = 0;
      if (const int next_frame_number =
              static_cast<int>(anim_frame_number_) + anim_arm_direction_;
          next_frame_number < 0 || next_frame_number > 4) {
        anim_arm_direction_ = -anim_arm_direction_;
      }
      anim_frame_number_ += anim_arm_direction_;
    }
  }

  // Check game end
  if (game_ended_) {
    if (state_ == PlayerState::Normal) {
      if (is_winner_) {
        state_ = PlayerState::Winner;

        // TODO: Sound
        // maybe-stereo-sound function FUN_00408470 (0x90) omitted:
        // refer to a detailed comment above about this function
        // maybe-sound code function (0x98 + 0x10) omitted
        // player.sound.pipikachu = true;
      }
      else {
        state_ = PlayerState::Loser;
      }
      anim_frame_delay_ = 0;
      anim_frame_number_ = 0;
    }
    // TODO
    // FUN_004025e0
    // Process game end frames (winner / loser animations)
    // processGameEndFrameFor(player);
    if (anim_frame_number_ < 4) {
      anim_frame_delay_++;
      if (anim_frame_delay_ > 4) {
        anim_frame_delay_ = 0;
        anim_frame_number_++;
      }
    }

  }
}

void Player::end_game(const bool is_winner) {
  game_ended_ = true;
  is_winner_ = is_winner;
}


} // namespace pika