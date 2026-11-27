#include <doctest/doctest.h>

#include <pikaball/physics/ball.hpp>
#include <pikaball/physics/player.hpp>
#include <pikaball/random.hpp>

#include "helpers.hpp"

using namespace pika;
using namespace pika::test;

namespace {

/** Random numbers for the hits (only used when the ball lands almost on the player's center) */
Rng hit_rng {0};

/** A ball placed for the start of a round on the given side */
Ball serve(const FieldSide side) {
  Ball ball;
  ball.initialize(side);
  return ball;
}

/** Update the ball n times */
void step(Ball& ball, const int n) {
  for (int i = 0; i < n; ++i) {
    ball.update();
  }
}

} // namespace

TEST_CASE("Ball: initialize places the ball on the serving side") {
  Ball left = serve(FieldSide::Left);
  CHECK(left.x() == 56);
  CHECK(left.y() == 0);
  CHECK(left.velocity_x() == 0);
  CHECK(left.velocity_y() == 1);
  CHECK(left.rotation() == 0);
  CHECK(left.punch_effect_radius() == 0);
  CHECK_FALSE(left.power_hit());
  CHECK(left.sound() == BallSound::None);

  Ball right = serve(FieldSide::Right);
  CHECK(right.x() == ground_width - 56);
  CHECK(right.y() == 0);
}

TEST_CASE("Ball: initialize resets a ball that was already played") {
  Ball ball = serve(FieldSide::Left);
  ball.set_velocity_x(20);
  step(ball, 10);
  ball.initialize(FieldSide::Right);
  CHECK(ball == serve(FieldSide::Right));
}

TEST_CASE("Ball: free fall and bounce on the ground") {
  Ball ball = serve(FieldSide::Left);

  // Gravity: the y velocity grows by 1 every frame
  CHECK_FALSE(ball.update());
  CHECK(ball.y() == 1);
  CHECK(ball.velocity_y() == 2);

  // update() returns true on the frame the ball touches the ground (frame 22)
  int frame = 1;
  bool touched_ground = false;
  while (!touched_ground && frame < 100) {
    touched_ground = ball.update();
    ++frame;
  }
  CHECK(frame == 22);
  CHECK(ball.x() == 56);
  CHECK(ball.y() == 252);  // Literal values from the original game, not the constants under test
  CHECK(ball.velocity_y() == -22);  // Bounces up with the same speed
  CHECK(ball.sound() == BallSound::Ground);
  // Punch effect drawn under the ball
  CHECK(ball.punch_effect_x() == 56);
  CHECK(ball.punch_effect_y() == 272);
  CHECK(ball.punch_effect_radius() == 20);

  // Next frame: the ball goes up and the punch effect shrinks
  CHECK_FALSE(ball.update());
  CHECK(ball.y() == ball_ground_y - 22);
  CHECK(ball.punch_effect_radius() == ball_radius - 2);
}

TEST_CASE("Ball: punch effect radius shrinks by 2 per frame, down to 0") {
  Ball ball = serve(FieldSide::Left);
  step(ball, 22);  // Touches the ground: radius 20
  REQUIRE(ball.punch_effect_radius() == 20);
  step(ball, 9);
  CHECK(ball.punch_effect_radius() == 2);
  ball.update();
  CHECK(ball.punch_effect_radius() == 0);
  ball.update();
  CHECK(ball.punch_effect_radius() == 0);
}

TEST_CASE("Ball: bounces off the walls") {
  SUBCASE("left wall") {
    Ball ball = serve(FieldSide::Left);
    ball.set_velocity_x(-30);
    ball.update();
    CHECK(ball.x() == 26);
    // Next position would be -4 (< ball radius): the x velocity is inverted
    ball.update();
    CHECK(ball.velocity_x() == 30);
    CHECK(ball.x() == 56);
  }
  SUBCASE("right wall") {
    Ball ball = serve(FieldSide::Right);
    ball.set_velocity_x(30);
    ball.update();
    CHECK(ball.x() == 406);
    // Next position would be 436 (> field width)
    ball.update();
    CHECK(ball.velocity_x() == -30);
    CHECK(ball.x() == 376);
  }
}

TEST_CASE("Ball: bounces off the ceiling") {
  Ball ball = serve(FieldSide::Left);
  ball.set_velocity_y(-10);
  ball.update();
  // Going above y = 0 resets the y velocity to 1 (going down)
  CHECK(ball.y() == 1);
  CHECK(ball.velocity_y() == 2);
}

TEST_CASE("Ball: bounces off the net") {
  SUBCASE("side of the net: bounces back horizontally") {
    Ball ball = serve(FieldSide::Left);
    ball.set_velocity_x(7);
    step(ball, 20);
    // Close to the net and below its top
    REQUIRE(ball.x() == 196);
    REQUIRE(ball.y() == 210);
    ball.update();
    CHECK(ball.velocity_x() == -7);
    CHECK(ball.x() == 189);
  }
  SUBCASE("top of the net: bounces up") {
    Ball ball = serve(FieldSide::Left);
    ball.set_velocity_x(8);
    step(ball, 19);
    // Over the net, between the top and bottom of the net top
    REQUIRE(ball.x() == 208);
    REQUIRE(ball.y() == 190);
    REQUIRE(ball.velocity_y() == 20);
    ball.update();
    CHECK(ball.velocity_y() == -19);  // Inverted, then gravity
    CHECK(ball.y() == 170);
    CHECK(ball.x() == 216);
  }
}

TEST_CASE("Ball: collision box with a player") {
  // Player at (36, 244): the ball collides if both distances are <= 32
  const Player player(FieldSide::Left);
  Ball ball = serve(FieldSide::Left);  // x = 56: 20 px from the player

  step(ball, 19);
  REQUIRE(ball.y() == 190);  // 54 px above the player
  CHECK_FALSE(ball.collision_with_player(player));
  ball.update();
  REQUIRE(ball.y() == 210);  // 34 px above
  CHECK_FALSE(ball.collision_with_player(player));
  ball.update();
  REQUIRE(ball.y() == 231);  // 13 px above
  CHECK(ball.collision_with_player(player));

  // Too far away horizontally
  const Player far_player(FieldSide::Right);
  CHECK_FALSE(ball.collision_with_player(far_player));
}

TEST_CASE("Ball: hit by a player on the ground") {
  SUBCASE("ball to the right of the player goes right") {
    const Player player(FieldSide::Left);  // x = 36
    Ball ball = serve(FieldSide::Left);  // x = 56
    ball.process_player_hit(player, input::idle(), hit_rng);
    CHECK(ball.velocity_x() == 6);    // distance 20 / 3
    CHECK(ball.velocity_y() == -15);  // at least 15 up
    CHECK_FALSE(ball.power_hit());
  }
  SUBCASE("ball to the left of the player goes left") {
    const Player player(FieldSide::Right);  // x = 396
    Ball ball = serve(FieldSide::Right);  // x = 376
    ball.process_player_hit(player, input::idle(), hit_rng);
    CHECK(ball.velocity_x() == -6);
  }
  SUBCASE("a fast ball keeps its speed, upwards") {
    const Player player(FieldSide::Left);
    Ball ball = serve(FieldSide::Left);
    ball.set_velocity_y(22);
    ball.process_player_hit(player, input::idle(), hit_rng);
    CHECK(ball.velocity_y() == -22);
  }
  SUBCASE("ball almost on the player center gets a random x velocity") {
    Player player(FieldSide::Left);
    step(player, input::move(DirX::Right), 3);
    REQUIRE(player.x() == 54);  // 2 px from the ball: 2 / 3 = 0
    for (std::uint32_t seed = 0; seed < 20; ++seed) {
      Rng seeded_rng {seed};
      Ball ball = serve(FieldSide::Left);
      ball.process_player_hit(player, input::idle(), seeded_rng);
      CHECK(ball.velocity_x() >= -1);
      CHECK(ball.velocity_x() <= 1);
    }
  }
  SUBCASE("the expected landing point is updated") {
    const Player player(FieldSide::Left);
    Ball ball = serve(FieldSide::Left);
    ball.process_player_hit(player, input::idle(), hit_rng);
    Ball copy = ball;
    copy.calculate_landing_point();
    CHECK(ball.expected_landing_x() == copy.expected_landing_x());
    CHECK(ball.expected_landing_x() != 0);
  }
}

TEST_CASE("Ball: power hit") {
  // A left player jumping and power hitting
  Player player(FieldSide::Left);
  player.update(input::jump());
  player.update(input::power_hit());
  REQUIRE(player.state() == PlayerState::PowerHit);

  SUBCASE("forward and down from the left side") {
    Ball ball = serve(FieldSide::Left);
    ball.set_velocity_y(10);
    ball.process_player_hit(player, input::power_hit(DirX::Right, DirY::Down), hit_rng);
    CHECK(ball.velocity_x() == 20);
    CHECK(ball.velocity_y() == 30);  // 2 * |-15|: the y speed is raised to 15 before the power hit
    CHECK(ball.power_hit());
    CHECK(ball.sound() == BallSound::Hit);
    CHECK(ball.punch_effect_radius() == ball_radius);
    CHECK(ball.punch_effect_x() == ball.x());
    CHECK(ball.punch_effect_y() == ball.y());
  }
  SUBCASE("no direction: half the x speed, flat") {
    Ball ball = serve(FieldSide::Left);
    ball.process_player_hit(player, input::power_hit(), hit_rng);
    CHECK(ball.velocity_x() == 10);
    CHECK(ball.velocity_y() == 0);
  }
  SUBCASE("up") {
    Ball ball = serve(FieldSide::Left);
    ball.set_velocity_y(20);
    ball.process_player_hit(player, input::power_hit(DirX::None, DirY::Up), hit_rng);
    CHECK(ball.velocity_y() == -40);
  }
  SUBCASE("from the right side the ball goes left, whatever the direction") {
    Ball ball = serve(FieldSide::Right);
    ball.process_player_hit(player, input::power_hit(DirX::Right), hit_rng);
    CHECK(ball.velocity_x() == -20);
  }
  SUBCASE("a ball exactly over the net goes left") {
    Ball ball = serve(FieldSide::Left);
    ball.set_velocity_x(20);
    step(ball, 8);
    REQUIRE(ball.x() == ground_h_width);
    ball.process_player_hit(player, input::power_hit(DirX::Right), hit_rng);
    CHECK(ball.velocity_x() == -20);
  }
}

TEST_CASE("Ball: the expected landing point matches the real trajectory") {
  for (const int velocity_x : {-20, -13, -7, 0, 3, 7, 8, 12, 20}) {
    for (const int velocity_y : {-30, -15, 1, 10}) {
      CAPTURE(velocity_x);
      CAPTURE(velocity_y);
      Ball ball = serve(FieldSide::Left);
      ball.set_velocity_x(velocity_x);
      ball.set_velocity_y(velocity_y);
      ball.calculate_landing_point();

      // Play the ball until it touches the ground
      Ball played = ball;
      int frames = 0;
      while (!played.update() && frames < 1000) {
        ++frames;
      }
      CHECK(ball.expected_landing_x() == played.x());
    }
  }
}

TEST_CASE("Ball: rotation and hyper ball") {
  SUBCASE("rotation stays in [0, 5] in both directions") {
    for (const int velocity_x : {-20, -7, 3, 13, 20}) {
      CAPTURE(velocity_x);
      Ball ball = serve(FieldSide::Left);
      ball.set_velocity_x(velocity_x);
      for (int frame = 0; frame < 200; ++frame) {
        ball.update();
        REQUIRE(ball.rotation() >= 0);
        REQUIRE(ball.rotation() <= 5);
      }
    }
  }
  SUBCASE("a fine rotation of exactly 50 shows the hyper ball sprite (rotation 5)") {
    Ball ball = serve(FieldSide::Left);
    ball.set_velocity_x(20);  // Fine rotation +10 per frame: 10, 20, 30, 40, 50
    step(ball, 4);
    CHECK(ball.rotation() == 4);
    ball.update();
    CHECK(ball.rotation() == 5);
    ball.update();  // 60 wraps to 10
    CHECK(ball.rotation() == 1);
  }
}

TEST_CASE("Ball: reset_sound clears the sound") {
  Ball ball = serve(FieldSide::Left);
  step(ball, 22);
  REQUIRE(ball.sound() == BallSound::Ground);
  ball.reset_sound();
  CHECK(ball.sound() == BallSound::None);
}
