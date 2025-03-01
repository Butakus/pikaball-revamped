#include "pikaball/controller/computer_controller.hpp"
#include <pikaball/random.hpp>

namespace pika {

ComputerController::ComputerController(const FieldSide &side) :
 PlayerController(side),
 is_player_right_(side == FieldSide::Right),
 left_bound_(is_player_right_ * ground_h_width),
 right_bound_((is_player_right_ + 1) * ground_h_width)
{}

void ComputerController::on_round_start(const PhysicsView &physics) {
  // At the start of each round, reset the boldness to a random value
  boldness_ = rand_int() % 5;
}

/**
 * Estimate where will the ball land if the player power hits with the given input.
 * FUN_00402870, expectedLandingPointXWhenPowerHit
 * @param input The player's input
 * @param ball The ball
 * @return The x coordinate of expected landing point
 */
unsigned int estimate_ball_hit_landing(const PlayerInput& input, const Ball& ball) {
  // First, estimate the velocity after the power hit.
  // This code is the same from the Ball physics
  // Base velocity is halved if no direction is pressed when power hitting
  int velocity_x = input.direction_x == DirX::None ? 10 : 20;
  if (ball.x() >= ground_h_width) {
    velocity_x = - velocity_x;
  }
  const int velocity_y = 2 * std::abs(ball.velocity_y()) * static_cast<int>(input.direction_y);

  // Clone the ball object
  Ball ball_clone = ball;
  // Set the velocities after the power hit
  ball_clone.set_velocity_x(velocity_x);
  ball_clone.set_velocity_y(velocity_y);
  // Estimate the new landing point
  ball_clone.calculate_landing_point();

  return ball_clone.expected_landing_x();
}

bool ComputerController::decide_input_power_hit(const PhysicsView& physics_view, PlayerInput& input) const {
  /* Two random strategies possible:
   * 1. Check the Y direction Down -> Middle -> Up
   * 2. Check the Y direction Up -> Middle -> Down (flip_dir_y)
   * The X direction is always checked in this order: Front -> None
   * The first combination of X/Y directions that finds a good hit will be returned.
   */
  const bool flip_dir_y = rand_int() % 2 == 0;
  for (int dir_x = 1; dir_x > -1; dir_x--) {
    for (int dir_y = 1; dir_y > -2; dir_y--) {
      PlayerInput check_input {
        .direction_x = static_cast<DirX>(dir_x),
        // Invert dir_y if the flag was randomly set
        .direction_y = static_cast<DirY>(flip_dir_y ? - dir_y : dir_y),
      };
      // With the test input, check where would the ball land
      const unsigned int land_x = estimate_ball_hit_landing(check_input, physics_view.ball);
      // Distance between the other player and the ball's landing point
      const int player_dist = static_cast<int>(land_x) - static_cast<int>(other_player_.x());
      /* The player will power hit if these conditions are met:
       * 1. The ball will land on the other side
       * 2. The ball will not land on the other player's position
       */
      if ((land_x <= left_bound_ || land_x >= right_bound_) &&
          std::abs(player_dist) > player_size) {
        input.direction_x = check_input.direction_x;
        input.direction_y = check_input.direction_y;
        return true;
      }
    }
  }
  return false;
}

PlayerInput ComputerController::on_update(const PhysicsView &physics_view) {
  PlayerInput input {};

  // Initialize some values for later
  ball_ = physics_view.ball;
  if (is_player_right_) {
    player_ = physics_view.player_right;
    other_player_ = physics_view.player_left;
  }
  else {
    player_ = physics_view.player_left;
    other_player_ = physics_view.player_right;
  }
  const unsigned int ball_land_x = ball_.expected_landing_x();
  const unsigned int ball_distance_x =
    std::abs(static_cast<int>(ball_.x()) - static_cast<int>(player_.x()));
  const unsigned int ball_distance_y =
    std::abs(static_cast<int>(ball_.y()) - static_cast<int>(player_.y()));
  // Target position for the player to move
  unsigned int target_x = ball_land_x;

  /* Try to stay in the middle of the side if these conditions are met:
   *  1. The idle position is set to the middle (value 0). This is set randomly.
   *  2. The ball is far from the player (distance_x > 100)
   *  3. The computer is bold enough (considering the ball's velocity)
   *  4. The ball is not going to land on our side
   */
  if (computer_idle_position_ == 0 &&
      ball_distance_x > 100 &&
      std::abs(ball_.velocity_x()) < boldness_ + 5 &&
      (ball_land_x <= left_bound_ || ball_land_x >= right_bound_)) {
    // Set the target x to the middle of our field side
    target_x = left_bound_ + ground_h_width / 2;
  }

  // If player is far from the target and is not bold enough... Go towards the target
  const int target_x_dist = std::abs(static_cast<int>(target_x) - static_cast<int>(player_.x()));
  if (target_x_dist > boldness_ + 8) {
    if (player_.x() < target_x) {
      input.direction_x = DirX::Right;
    }
    else {
      input.direction_x = DirX::Left;
    }
  }
  else if (rand_int() % 20 == 0) {
    computer_idle_position_ = rand_int() % 2;
  }
  if (player_.state() == PlayerState::Normal) {
    // If the player is on the ground, decide whether to jump, or dive

    /* The computer decides to jump if these conditions are met:
     * 1. Ball is moving slow and the player is not very bold
     * 2. Ball is on top of the player
     * 3. Ball is going down
     * 4. Ball is high. The bolder the player, the lower the ball can be to jump.
     */
    if (std::abs(ball_.velocity_x()) < boldness_ + 3 &&
        ball_distance_x < player_h_size &&
        ball_.velocity_y() > 0 &&
        ball_.y() < 10 * boldness_ + 84) {
      input.direction_y = DirY::Up;
    }

    /* The computer decides to dive if these conditions are met:
     * 1. The ball is landing on our side
     * 2. The ball is currently on our side
     * 3. Ball is far from the player and we are not bold enough
     * 4. Ball is close to the ground (lower than 174)
     */
    if (ball_land_x > left_bound_ && ball_land_x < right_bound_ &&
        ball_.x() > left_bound_ && ball_.x() < right_bound_ &&
        ball_distance_x > boldness_ * 5 + player_size &&
        ball_.y() > 174) {
      input.power_hit = true;
      if (player_.x() < ball_.x()) {
        input.direction_x = DirX::Right;
      }
      else {
        input.direction_x = DirX::Left;
      }
    }
  }
  else if (player_.state() == PlayerState::Jumping || player_.state() == PlayerState::PowerHit) {
    // If the player is jumping or power hitting...
    if (ball_distance_x > 8) {
      // If the ball is far, move towards it
      if (player_.x() < ball_.x()) {
        input.direction_x = DirX::Right;
      } else {
        input.direction_x = DirX::Left;
      }
    }
    if (ball_distance_x < 48 && ball_distance_y < 48) {
      // If the ball is close, we can power hit. Check it and update input direction.
      if (decide_input_power_hit(physics_view, input)) {
        input.power_hit = true;
        const int player_dist =
          std::abs(static_cast<int>(other_player_.x()) - static_cast<int>(player_.x()));
        if (player_dist < 80 && input.direction_y != DirY::Up) {
          // If the other player is too close, send the ball up to avoid blocking
          input.direction_y = DirY::Up;
        }
      }
    }
  }

  return input;
}


} // namespace pika