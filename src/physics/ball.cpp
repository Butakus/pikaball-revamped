#include "ball.hpp"
#include <cmath>

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
    return true;
  }

  // Update position
  y_ = next_y;
  x_ = next_x;
  // Gravity effect
  velocity_y_++;

  return false;
}


} // pika