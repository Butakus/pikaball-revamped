#include "pikaball/controller/keyboard_controller.hpp"

namespace pika {

KeyboardController::KeyboardController(const FieldSide &side) : PlayerController(side) {}


void KeyboardController::set_input(const PlayerInput &input) {
  input_ = input;
}

PlayerInput KeyboardController::update(const PhysicsView &) {
  return input_;
}

} // namespace pika
