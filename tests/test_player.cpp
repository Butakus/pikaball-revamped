#include <doctest/doctest.h>

#include <pikaball/physics/player.hpp>

#include <algorithm>

#include "helpers.hpp"

using namespace pika;
using namespace pika::test;

namespace {

/** Update the player n times with the same input */
void step(Player& player, const PlayerInput& input, const int n) {
  for (int i = 0; i < n; ++i) {
    player.update(input);
  }
}

} // namespace

TEST_CASE("Player: start position depends on the field side") {
  const Player left(FieldSide::Left);
  CHECK(left.x() == 36);
  CHECK(left.y() == player_ground_y);
  CHECK(left.state() == PlayerState::Normal);
  CHECK(left.sound() == PlayerSound::None);
  CHECK(left.side() == FieldSide::Left);

  const Player right(FieldSide::Right);
  CHECK(right.x() == ground_width - 36);
  CHECK(right.y() == player_ground_y);
  CHECK(right.side() == FieldSide::Right);
}

TEST_CASE("Player: walks 6 px per frame") {
  Player player(FieldSide::Left);
  player.update(input::move(DirX::Right));
  CHECK(player.x() == 42);
  step(player, input::move(DirX::Right), 4);
  CHECK(player.x() == 66);
  player.update(input::move(DirX::Left));
  CHECK(player.x() == 60);
  // Walking doesn't change the height or the state
  CHECK(player.y() == player_ground_y);
  CHECK(player.state() == PlayerState::Normal);
}

TEST_CASE("Player: stays inside its half of the field") {
  SUBCASE("left player") {
    Player player(FieldSide::Left);
    step(player, input::move(DirX::Left), 50);
    CHECK(player.x() == player_h_size);  // 32
    step(player, input::move(DirX::Right), 50);
    CHECK(player.x() == ground_h_width - player_h_size);  // 184
  }
  SUBCASE("right player") {
    Player player(FieldSide::Right);
    step(player, input::move(DirX::Left), 50);
    CHECK(player.x() == ground_h_width + player_h_size);  // 248
    step(player, input::move(DirX::Right), 50);
    CHECK(player.x() == ground_width - player_h_size);  // 400
  }
}

TEST_CASE("Player: jump") {
  Player player(FieldSide::Left);

  // Frame 1: jump starts with velocity -16
  player.update(input::jump());
  CHECK(player.state() == PlayerState::Jumping);
  CHECK(player.sound() == PlayerSound::Chu);
  CHECK(player.y() == player_ground_y - 16);

  // Peak at y = 108 (136 px above the ground).
  // Reaches the ground on frame 33, but stays Jumping on that frame and lands on frame 34.
  int min_y = player.y();
  int frame = 1;
  while (player.state() != PlayerState::Normal && frame < 100) {
    player.update(input::idle());
    ++frame;
    min_y = std::min(min_y, player.y());
  }
  CHECK(min_y == 108);
  CHECK(frame == 34);
  CHECK(player.y() == player_ground_y);
}

TEST_CASE("Player: can't jump again while in the air") {
  Player player(FieldSide::Left);
  player.update(input::jump());
  player.update(input::idle());
  const int y_going_up = player.y();

  // Pressing up again in the air doesn't restart the jump
  player.update(input::jump());
  CHECK(player.state() == PlayerState::Jumping);
  CHECK(player.y() < y_going_up);
  CHECK(player.y() == y_going_up - 14);  // velocity -16 → -15 → -14: the jump continues normally
}

TEST_CASE("Player: holding up jumps again right after landing") {
  Player player(FieldSide::Left);
  step(player, input::jump(), 33);
  // On the ground on frame 33, but still Jumping
  CHECK(player.state() == PlayerState::Jumping);
  CHECK(player.y() == player_ground_y);

  // Frame 34: jumps again without landing first
  player.update(input::jump());
  CHECK(player.state() == PlayerState::Jumping);
  CHECK(player.y() == player_ground_y - 16);
}

TEST_CASE("Player: power hit on the frame the jump reaches the ground") {
  Player player(FieldSide::Left);
  step(player, input::jump(), 1);
  step(player, input::idle(), 31);
  // Frame 33: on the ground but still Jumping, so a power hit is not a dive
  player.update(input::dive(DirX::Right));
  CHECK(player.state() == PlayerState::PowerHit);
  CHECK(player.y() == player_ground_y);
  // Frame 34: lands
  player.update(input::idle());
  CHECK(player.state() == PlayerState::Normal);
}

TEST_CASE("Player: power hit in the air") {
  Player player(FieldSide::Left);
  player.update(input::jump());

  // Frame 2: power hit
  player.update(input::power_hit());
  CHECK(player.state() == PlayerState::PowerHit);
  CHECK(player.sound() == PlayerSound::Pika);

  // The power hit animation (5 frames of delay, then animation frames 1 to 4) ends 9 frames after the hit:
  // the player is in PowerHit on frames 2 to 10 and back to Jumping on frame 11
  int frames = 0;
  while (player.state() == PlayerState::PowerHit && frames < 100) {
    player.update(input::idle());
    ++frames;
  }
  CHECK(frames == 9);
  CHECK(player.state() == PlayerState::Jumping);
}

TEST_CASE("Player: power hit on the ground without a direction does nothing") {
  Player player(FieldSide::Left);
  player.update(input::power_hit());
  CHECK(player.state() == PlayerState::Normal);
  CHECK(player.sound() == PlayerSound::None);
  CHECK(player.x() == 36);
  CHECK(player.y() == player_ground_y);
}

TEST_CASE("Player: dive") {
  Player player(FieldSide::Left);

  // Frame 1: the dive starts. The player still walks 6 px on this frame.
  player.update(input::dive(DirX::Right));
  CHECK(player.state() == PlayerState::Diving);
  CHECK(player.sound() == PlayerSound::Chu);
  CHECK(player.diving_direction() == DirX::Right);
  CHECK(player.x() == 42);

  // Then it slides 8 px per frame in the dive direction, ignoring the input.
  // It reaches the ground on frame 12 but stays Diving on that frame, and lands on frame 13.
  player.update(input::move(DirX::Left));
  CHECK(player.x() == 50);
  int frame = 2;
  while (player.state() == PlayerState::Diving && frame < 100) {
    player.update(input::idle());
    ++frame;
  }
  CHECK(frame == 13);
  CHECK(player.x() == 138);
  CHECK(player.y() == player_ground_y);
  CHECK(player.state() == PlayerState::AfterDiving);

  // Lying on the ground for 5 frames, without moving, then back to Normal
  step(player, input::move(DirX::Left), 4);
  CHECK(player.state() == PlayerState::AfterDiving);
  CHECK(player.x() == 138);
  player.update(input::idle());
  CHECK(player.state() == PlayerState::Normal);
}

TEST_CASE("Player: dive to the left") {
  Player player(FieldSide::Right);
  player.update(input::dive(DirX::Left));
  CHECK(player.diving_direction() == DirX::Left);
  CHECK(player.x() == ground_width - 36 - 6);
  player.update(input::idle());
  CHECK(player.x() == ground_width - 36 - 6 - 8);
}

TEST_CASE("Player: end of game animations") {
  SUBCASE("winner") {
    Player player(FieldSide::Right);
    player.end_game(true);
    player.update(input::idle());
    CHECK(player.state() == PlayerState::Winner);
    CHECK(player.sound() == PlayerSound::Pipikachu);

    // The animation advances one frame every 5 updates, up to frame 4, then stays there
    step(player, input::idle(), 18);
    CHECK(player.anim_frame_number() == 3);
    player.update(input::idle());
    CHECK(player.anim_frame_number() == 4);
    step(player, input::idle(), 20);
    CHECK(player.anim_frame_number() == 4);
    CHECK(player.state() == PlayerState::Winner);
  }
  SUBCASE("loser") {
    Player player(FieldSide::Left);
    player.end_game(false);
    player.update(input::idle());
    CHECK(player.state() == PlayerState::Loser);
    CHECK(player.sound() == PlayerSound::None);
  }
  SUBCASE("a jumping player waits until landing") {
    Player player(FieldSide::Left);
    player.update(input::jump());
    player.end_game(true);
    // Still Jumping on frame 33
    step(player, input::idle(), 32);
    CHECK(player.state() == PlayerState::Jumping);
    // Lands on frame 34 and goes straight to Winner
    player.update(input::idle());
    CHECK(player.y() == player_ground_y);
    CHECK(player.state() == PlayerState::Winner);
  }
}

TEST_CASE("Player: initialize_round resets position, state and sound") {
  Player player(FieldSide::Left);
  step(player, input::move(DirX::Right), 5);
  player.update(input::jump());

  player.initialize_round();
  CHECK(player.x() == 36);
  CHECK(player.y() == player_ground_y);
  CHECK(player.state() == PlayerState::Normal);
  CHECK(player.sound() == PlayerSound::None);
}

TEST_CASE("Player: reset_sound clears the sound") {
  Player player(FieldSide::Left);
  player.update(input::jump());
  REQUIRE(player.sound() == PlayerSound::Chu);
  player.reset_sound();
  CHECK(player.sound() == PlayerSound::None);
}
