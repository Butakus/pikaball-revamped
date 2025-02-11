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

/**
 * Utility function to extract the x direction from a PlayerInput struct.
 * @param input The player input
 * @return Integer with the values [-1, 0, 1] (left, nothing, right)
 */
inline int get_input_direction_x(const PlayerInput& input) {
  // Convert the left/right input keys to a [-1, 0, 1] integer
  int input_direction_x = 0;
  if (input.left) {
    input_direction_x = -1;
  }
  else if (input.right) {
    input_direction_x = 1;
  }
  return input_direction_x;
}
/**
 * Utility function to extract the y direction from a PlayerInput struct.
 * @param input The player input
 * @return Integer with the values [-1, 0, 1] (up, nothing, down)
 */
inline int get_input_direction_y(const PlayerInput& input) {
  // Convert the left/right input keys to a [-1, 0, 1] integer
  int input_direction_y = 0;
  if (input.up) {
    input_direction_y = -1;
  }
  else if (input.down) {
    input_direction_y = 1;
  }
  return input_direction_y;
}

} // namespace pika

#endif // PIKA_INPUT_HPP
