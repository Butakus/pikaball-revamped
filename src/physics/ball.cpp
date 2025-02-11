#include "ball.hpp"
#include <cmath>
#include <pikaball/random.hpp>

namespace pika {

void Ball::initialize(const FieldSide& field_side) {
  x_ = (field_side == FieldSide::Left) ? 56 : ground_width - 56;
  y_ = 0;
  velocity_x_ = 0;
  velocity_y_ = 1;
  punch_effect_radius_ = 0;
  power_hit_ = false;

  // The reset of the variables below is not included in the original game not the js version
  rotation_ = 0;
  fine_rotation_ = 0;
  punch_effect_x_ = 0;
  punch_effect_y_ = 0;
  punch_effect_radius_ = 0;
  power_hit_ = false;
  expected_landing_x_ = 0;
  trailing_x_ = {};
  trailing_y_ = {};
}

bool Ball::update() {
  // This is not part of this function in the original assembly code.
  // In the original assembly code, it is processed in other function (FUN_00402ee0)
  // But it is proper to process here.
  trailing_x_[1] = trailing_x_[0];
  trailing_y_[1] = trailing_y_[0];
  trailing_x_[0] = x_;
  trailing_y_[0] = y_;

  int next_fine_rotation = static_cast<int>(fine_rotation_) + velocity_x_ / 2;
  // If next_fine_rotation === 50, it skips next if statement finely.
  // Then fine_rotation_ = 50, and then rotation_ = 5 (which designates hyper ball sprite!).
  // In this way, hyper ball glitch occur!
  // If this happens at the end of round,
  // since velocity_x_ is 0-initialized at each start of round,
  // hyper ball sprite is rendered continuously until a collision happens.
  if (next_fine_rotation < 0) {
    next_fine_rotation += 50;
  }
  if (next_fine_rotation > 50) {
    next_fine_rotation += -50;
  }
  fine_rotation_ = next_fine_rotation;
  rotation_ = (fine_rotation_ / 10) | 0;

  return update_position();
}

bool Ball::update_position() {
  const unsigned int next_x = x_ + velocity_x_;
  /*
    If the center of ball would get out of left world bound or right world bound, bounce back.

    In this if statement, when considering left-right symmetry,
    "next_x > ground_width" should be changed to "next_x > (ground_width - ball_radius)",
    or "next_x < ball_radius" should be changed to "next_x < 0".
    Maybe the former one is more proper when seeing Pikachu player's x-direction boundary.
    Is this a mistake of the author of the original game?
    Or, was it set to this value to resolve infinite loop problem? (See comments on the constant INFINITE_LOOP_LIMIT.)
    If apply (next_x > (ground_width - ball_radius)), and if the maximum number of loop is not limited,
    it is observed that infinite loop in the function expectedLandingPointXWhenPowerHit does not terminate.
  */
  if (next_x < ball_radius || next_x > ground_width) {
    velocity_x_ = - velocity_x_;
  }

  int next_y = static_cast<int>(y_) + velocity_y_;
  // Check if the ball touches the ceiling
  if (next_y < 0) {
    velocity_y_ = 1;
  }

  // Check if ball touches the net
  const auto net_distance = (x_ > ground_h_width) ? x_ - ground_h_width
                                                  : ground_h_width - x_;
  if (net_distance < net_pillar_h_width && y_ > net_top_top_y) {
    if (y_ <= net_top_bottom_y) {
      // The ball collides with the top of the net and bounces back up
      if (velocity_y_ > 0) {
        velocity_y_ = -velocity_y_;
      }
    }
    else {
      // The ball collides with the net and bounces back left/right
      if (x_ < ground_h_width) {
        // TODO: I think this conditional produces the glitch that makes the ball pierce the net!
        velocity_x_ = -std::abs(velocity_x_);
      }
      else {
        velocity_x_ = std::abs(velocity_x_);
      }
    }
  }

  next_y = static_cast<int>(y_) + velocity_y_;
  // Check if the ball touches the ground
  bool ground_hit = false;
  if (next_y > ball_ground_y) {
    // FUN_00408470 omitted
    // the function omitted above receives 100 * (x_ - 216),
    // i.e. horizontal displacement from net maybe for stereo sound?
    // code function (ballpointer + 0x28 + 0x10)? omitted
    // the omitted two functions maybe do a part of sound playback role.

    // ball.sound.ballTouchesGround = true;

    velocity_y_ = -velocity_y_;
    punch_effect_x_ = x_;
    y_ = ball_ground_y;
    punch_effect_radius_ = ball_radius;
    punch_effect_y_ = ball_ground_y + ball_radius;
    ground_hit = true;
  }
  else {
    // Update position
    y_ = next_y;
    x_ = next_x;
    // Gravity effect
    velocity_y_++;
  }
  return ground_hit;
}

bool Ball::collision_with_player(const Player& player) const {
  const int diff_x = static_cast<int>(x_) - static_cast<int>(player.x());
  const int diff_y = static_cast<int>(y_) - static_cast<int>(player.y());
  return std::abs(diff_x) <= player_h_size && std::abs(diff_y) <= player_h_size;
}

void Ball::process_player_hit(const Player &player, const PlayerInput &input) {
  if (player.state() == PlayerState::PowerHit) {
    // Player is jumping and power hitting
    const int input_direction_x = get_input_direction_x(input);
    const int input_direction_y = get_input_direction_y(input);
    if (x_ < ground_h_width) {
      velocity_x_ = (std::abs(input_direction_x) + 1) * 10;
    }
    else {
      velocity_x_ = -(std::abs(input_direction_x) + 1) * 10;
    }
    punch_effect_x_ = x_;
    punch_effect_y_ = y_;

    velocity_y_ = std::abs(velocity_y_) * input_direction_y * 2;
    punch_effect_radius_ = ball_radius;

    // TODO: Sound
    // maybe-stereo-sound function FUN_00408470 (0x90) omitted:
    // refer to a detailed comment above about this function
    // maybe-soundcode function (ballpointer + 0x24 + 0x10) omitted:
    // ball.sound.powerHit = true;

    power_hit_ = true;
  }
  else {
    // Player is on the ground and ball hits the player
    // The x velocity depends on the distance to the center of the player
    const int diff_x = static_cast<int>(x_) - static_cast<int>(player.x());
    const int abs_distance = std::abs(diff_x);
    if (x_ < player.x()) {
      velocity_x_ = - (abs_distance / 3);
    }
    else if (x_ > player.x()) {
      velocity_x_ = abs_distance / 3;
    }

    if (velocity_x_ == 0) {
      // If ball velocity x is 0, randomly choose one of -1, 0, 1.
      velocity_x_ = rand_int() % 3 - 1;
    }

    const int abs_velocity_y = std::abs(velocity_y_);
    velocity_y_ = - abs_velocity_y;

    if (abs_velocity_y < 15) {
      velocity_y_ = -15;
    }

    power_hit_ = false;
  }

  // After updating the velocities, estimate next landing point
  calculate_landing_point();
}


void Ball::calculate_landing_point() {
  Ball ball_clone = *this;
  size_t loop_counter = 0;
  while (true) {
    loop_counter++;
    if (ball_clone.update_position() || loop_counter >= infinite_loop_limit) {
      break;
    }
  }
  expected_landing_x_ = ball_clone.x_;
}

} // pika