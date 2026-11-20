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
  [[nodiscard]] PlayerInput on_update(const PhysicsView&) override;

  /**
   * Store the current local input (from the game's keyboard handling).
   * This input will then just be forwarded when on_update is called.
   * @param input The local player input for this controller's side.
   */
  void on_local_input(const PlayerInput& input) override;

private:
  PlayerInput input_ {};
};

} // namespace pika

#endif // PIKA_KEYBOARD_CONTROLLER_HPP
