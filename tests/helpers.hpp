#ifndef PIKA_TESTS_HELPERS_HPP
#define PIKA_TESTS_HELPERS_HPP

#include <pikaball/input.hpp>
#include <pikaball/physics/player.hpp>

namespace pika::test {

/** Input builders for the players, used as input::idle(), input::jump()... */
namespace input {

/** No key pressed */
constexpr PlayerInput idle() {
  return {.direction_x = DirX::None, .direction_y = DirY::None, .power_hit = false};
}

/** Walk left or right */
constexpr PlayerInput move(const DirX direction) {
  return {.direction_x = direction, .direction_y = DirY::None, .power_hit = false};
}

/** Press up (jump) */
constexpr PlayerInput jump() {
  return {.direction_x = DirX::None, .direction_y = DirY::Up, .power_hit = false};
}

/** Power hit with the given directions (on the ground with a direction, this is a dive) */
constexpr PlayerInput power_hit(const DirX direction_x = DirX::None, const DirY direction_y = DirY::None) {
  return {.direction_x = direction_x, .direction_y = direction_y, .power_hit = true};
}

/** Dive towards the given direction (power hit + direction, on the ground) */
constexpr PlayerInput dive(const DirX direction) {
  return power_hit(direction);
}

} // namespace input

/** Update the player n times with the same input */
inline void step(Player& player, const PlayerInput& input, const int n) {
  for (int i = 0; i < n; ++i) {
    player.update(input);
  }
}

} // namespace pika::test

#endif // PIKA_TESTS_HELPERS_HPP
