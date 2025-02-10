#include "player.hpp"
#include "pikaball/random.hpp"

namespace pika {
Player::Player(const FieldSide &field_side, const bool is_computer) {
  field_side_ = field_side;
  is_computer_ = is_computer;

  initialize();
}

void Player::initialize() {
  x_ = (field_side_ == FieldSide::Left) ? 36 : ground_width - 36;
  y_ = player_ground_y;
  velocity_y_ = 0;

  state_ = PlayerState::Normal;

  anim_frame_number_ = 0;
  anim_arm_direction_ = 1;
  anim_frame_delay_ = 0;

  if (is_computer_) {
    computer_boldness_ = rand_int() % 5;
  }
}


} // namespace pika