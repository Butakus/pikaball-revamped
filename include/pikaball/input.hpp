#ifndef PIKA_INPUT_HPP
#define PIKA_INPUT_HPP

namespace pika {

struct MenuInput {
  bool up;
  bool down;
  bool enter_left;
  bool enter_right;
  bool enter;  // Will be true if any of the other two are true
};

enum class DirX {
  Left = -1,
  None = 0,
  Right = 1,
};

enum class DirY {
  Up = -1,
  None = 0,
  Down = 1,
};

struct PlayerInput {
  DirX direction_x;
  DirY direction_y;
  bool power_hit;
};

// OLD version with all keys. Not used anymore
// struct PlayerInput {
//   bool left;
//   bool right;
//   bool up;
//   bool down;
//   bool power_hit;
//   // Disabled. Originally only for player 1 (V key)
//   // bool down_right;
// };

/**
 * Utility function to extract the x direction from a PlayerInput struct.
 * @param input_left The value of the input left key
 * @param input_right The value of the input right key
 * @return DirX object with the values [-1, 0, 1] (Left, None, Right)
 */
inline DirX get_input_direction_x(const bool input_left, const bool input_right) {
  // Convert the left/right input keys to a DirX value
  DirX direction_x = DirX::None;
  if (input_left) {
    direction_x = DirX::Left;
  }
  else if (input_right) {
    direction_x = DirX::Right;
  }
  return direction_x;
}

/**
 * Utility function to extract the x direction from a PlayerInput struct.
 * @param input_up The value of the input up key
 * @param input_down The value of the input down key
 * @return Integer with the values [-1, 0, 1] (up, nothing, down)
 */
inline DirY get_input_direction_y(const bool input_up, const bool input_down) {
  // Convert the up/down input keys to a DirY value
  DirY direction_y = DirY::None;
  if (input_up) {
    direction_y = DirY::Up;
  }
  else if (input_down) {
    direction_y = DirY::Down;
  }
  return direction_y;
}

} // namespace pika

#endif // PIKA_INPUT_HPP
