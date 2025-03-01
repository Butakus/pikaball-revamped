#include <pikaball/physics/physics.hpp>

namespace pika {

Physics::Physics() :
  player_left_(FieldSide::Left),
  player_right_(FieldSide::Right)
{}

void Physics::init_round(const FieldSide &field_side) {
  ball_.initialize(field_side);
  player_left_.initialize_round();
  player_right_.initialize_round();
}

void Physics::restart() {
  ball_.initialize(FieldSide::Left);
  player_left_.initialize_game();
  player_right_.initialize_game();
}

bool Physics::update(const PlayerInput& input_left,
                     const PlayerInput& input_right) {
  // Update ball position and calculate estimated landing point
  const bool ball_touching_ground = ball_.update();
  ball_.calculate_landing_point();

  // Update player positions
  update_player(player_left_, input_left);
  update_player(player_right_, input_right);

  // Check collision between ball and players and process it
  collision_ball_player(player_left_, input_left);
  collision_ball_player(player_right_, input_right);

  return ball_touching_ground;
}

void Physics::end_game(const FieldSide& field_side) {
  player_left_.end_game(field_side == FieldSide::Left);
  player_right_.end_game(field_side == FieldSide::Right);
}

void Physics::update_player(Player &player, const PlayerInput &input) {
  if (player.is_computer()) {
    // TODO
  }
  else {
    player.update(input);
  }
}


void Physics::collision_ball_player(Player& player, const PlayerInput& input) {
  if (ball_.collision_with_player(player)) {
    if (!player.collision_with_ball) {
      ball_.process_player_hit(player, input);
      player.collision_with_ball = true;
    }
  }
  else {
    player.collision_with_ball = false;
  }
}

const Player& Physics::player(const FieldSide& side) const {
  return (side == FieldSide::Left) ? player_left_ : player_right_;
}


} // namespace pika
