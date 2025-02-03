#ifndef PIKA_INPUT_HPP
#define PIKA_INPUT_HPP

namespace pika {

struct PlayerInput {
  bool left;
  bool right;
  bool up;
  bool down;
  bool power_hit;
  // Disabled. Originally only for player 1 (V key)
  // bool down_right;
};

struct MenuInput {
  bool up;
  bool down;
  bool enter;
};

} // namespace pika

#endif // PIKA_INPUT_HPP
