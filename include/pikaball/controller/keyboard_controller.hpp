#ifndef PIKA_KEYBOARD_CONTROLLER_HPP
#define PIKA_KEYBOARD_CONTROLLER_HPP
#include "player_controller.hpp"

namespace pika {

class KeyboardController final : public PlayerController {
public:
  explicit KeyboardController(const FieldSide& side);
  ~KeyboardController() override = default;

  /**
   * Main controller function.
   * This function will be called by the game at each frame when the world is updated.
   * The param PhysicsView is ignored.
   * @return The player input for the player that is being controlled according to the keyboard state.
   */
  [[nodiscard]] PlayerInput update(const PhysicsView&) override;

  /**
   * Set the current user input from main SDL event handler.
   * This input will then just be forwarded when update is called.
   * @param input The PlayerInput state set by the event handler.
   */
  void set_input(const PlayerInput& input);

private:
  PlayerInput input_ {};
};

} // namespace pika

#endif // PIKA_KEYBOARD_CONTROLLER_HPP
