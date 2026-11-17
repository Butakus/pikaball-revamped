#include <doctest/doctest.h>

#include <pikaball/physics/physics.hpp>
#include <pikaball/random.hpp>

#include <vector>

#include "helpers.hpp"

using namespace pika;
using namespace pika::test;

namespace {

/** Random but valid input, drawn from the game's RNG (use seed_rng() for repeatable runs) */
PlayerInput random_input() {
  return {
    .direction_x = static_cast<DirX>(rand_int() % 3 - 1),
    .direction_y = static_cast<DirY>(rand_int() % 3 - 1),
    .power_hit = rand_int() % 4 == 0,
  };
}

/** Run a seeded game with random inputs, starting a new round after each point. Returns the state of every frame. */
std::vector<PhysicsView> simulate(const std::uint32_t seed, const int frames) {
  seed_rng(seed);
  Physics physics;
  physics.init_round(FieldSide::Left);
  std::vector<PhysicsView> states;
  states.reserve(frames);
  for (int frame = 0; frame < frames; ++frame) {
    if (physics.update(random_input(), random_input())) {
      physics.init_round(physics.ball().x() < ground_h_width ? FieldSide::Right : FieldSide::Left);
    }
    states.emplace_back(physics);
  }
  return states;
}

} // namespace


TEST_CASE("Physics: a new round places the ball on the serving side") {
  Physics physics;

  physics.init_round(FieldSide::Left);
  CHECK(physics.ball().x() == 56);
  CHECK(physics.ball().y() == 0);

  physics.init_round(FieldSide::Right);
  CHECK(physics.ball().x() == ground_width - 56);
  CHECK(physics.ball().y() == 0);

  // Players go back to their start positions
  CHECK(physics.player(FieldSide::Left).x() == 36);
  CHECK(physics.player(FieldSide::Right).x() == ground_width - 36);
  CHECK(physics.player(FieldSide::Left).y() == player_ground_y);
  CHECK(physics.player(FieldSide::Right).y() == player_ground_y);
}

TEST_CASE("Physics: restart resets players and serves from the left") {
  Physics physics;
  physics.init_round(FieldSide::Right);
  // Move the players and end the game
  for (int i = 0; i < 10; ++i) {
    physics.update(input::move(DirX::Right), input::move(DirX::Left));
  }
  physics.end_game(FieldSide::Left);

  physics.restart();
  CHECK(physics.ball().x() == 56);
  CHECK(physics.player(FieldSide::Left).x() == 36);
  CHECK(physics.player(FieldSide::Right).x() == ground_width - 36);

  // The game-ended flag is cleared: players stay Normal after an update
  physics.update(input::idle(), input::idle());
  CHECK(physics.player(FieldSide::Left).state() == PlayerState::Normal);
  CHECK(physics.player(FieldSide::Right).state() == PlayerState::Normal);
}

TEST_CASE("Physics: the serve bounces off an idle player and lands on the other side") {
  Physics physics;
  physics.init_round(FieldSide::Left);

  // The ball falls straight down onto the left player (x 56 vs 36: within the 32 px collision box)
  for (int frame = 1; frame <= 20; ++frame) {
    CHECK_FALSE(physics.update(input::idle(), input::idle()));
    CHECK(physics.ball().x() == 56);
  }

  // Frame 21: the ball touches the player and bounces up and to the right
  CHECK_FALSE(physics.update(input::idle(), input::idle()));
  CHECK(physics.player(FieldSide::Left).collision_with_ball);
  CHECK(physics.ball().velocity_x() == 6);    // distance to the player center (20) / 3
  CHECK(physics.ball().velocity_y() == -22);
  CHECK(physics.ball().expected_landing_x() == 272);

  // The ball is hit only once: the flag is cleared when the ball leaves the player
  physics.update(input::idle(), input::idle());
  CHECK_FALSE(physics.player(FieldSide::Left).collision_with_ball);
  CHECK(physics.ball().velocity_x() == 6);

  // It lands where it was expected, on the right side, on frame 58
  int frame = 22;
  bool touched_ground = false;
  while (!touched_ground && frame < 200) {
    ++frame;
    touched_ground = physics.update(input::idle(), input::idle());
  }
  CHECK(touched_ground);
  CHECK(frame == 58);
  CHECK(physics.ball().x() == 272);
  CHECK(physics.ball().y() == ball_ground_y);
  CHECK(physics.ball().sound() == BallSound::Ground);
}

TEST_CASE("Physics: end_game sets the winner and loser animations") {
  Physics physics;
  physics.init_round(FieldSide::Left);

  physics.end_game(FieldSide::Right);
  physics.update(input::idle(), input::idle());

  CHECK(physics.player(FieldSide::Left).state() == PlayerState::Loser);
  CHECK(physics.player(FieldSide::Right).state() == PlayerState::Winner);
  CHECK(physics.player(FieldSide::Right).sound() == PlayerSound::Pipikachu);
  CHECK(physics.player(FieldSide::Left).sound() == PlayerSound::None);
}

TEST_CASE("Physics: reset_sound clears the ball and player sounds") {
  Physics physics;
  physics.init_round(FieldSide::Left);
  physics.update(input::jump(), input::jump());
  REQUIRE(physics.player(FieldSide::Left).sound() == PlayerSound::Chu);

  physics.reset_sound();
  CHECK(physics.player(FieldSide::Left).sound() == PlayerSound::None);
  CHECK(physics.player(FieldSide::Right).sound() == PlayerSound::None);
  CHECK(physics.ball().sound() == BallSound::None);
}

TEST_CASE("Physics: the same seed and inputs give the same game") {
  constexpr int frames = 3000;
  const auto first = simulate(42, frames);
  const auto second = simulate(42, frames);

  // Compare the whole state (ball and both players) frame by frame, and stop at the first difference
  for (int frame = 0; frame < frames; ++frame) {
    CAPTURE(frame);
    REQUIRE(first[frame] == second[frame]);
  }

  // A different seed gives a different game (sanity check that the inputs are really random)
  const auto other = simulate(43, frames);
  CHECK_FALSE(first == other);
}

TEST_CASE("Physics: positions stay inside the field during a long random game") {
  seed_rng(1234);
  Physics physics;
  physics.init_round(FieldSide::Left);

  int points = 0;
  for (int frame = 0; frame < 10000; ++frame) {
    if (physics.update(random_input(), random_input())) {
      ++points;
      physics.init_round(physics.ball().x() < ground_h_width ? FieldSide::Right : FieldSide::Left);
      continue;
    }
    const Ball& ball = physics.ball();
    REQUIRE(ball.x() >= ball_radius);
    REQUIRE(ball.x() <= ground_width);
    REQUIRE(ball.y() >= 0);
    REQUIRE(ball.y() <= ball_ground_y);

    const Player& left = physics.player(FieldSide::Left);
    const Player& right = physics.player(FieldSide::Right);
    REQUIRE(left.x() >= player_h_size);
    REQUIRE(left.x() <= ground_h_width - player_h_size);
    REQUIRE(right.x() >= ground_h_width + player_h_size);
    REQUIRE(right.x() <= ground_width - player_h_size);
    REQUIRE(left.y() <= player_ground_y);
    REQUIRE(right.y() <= player_ground_y);
  }
  // Points are actually scored (the simulation doesn't get stuck)
  CHECK(points > 100);
}
