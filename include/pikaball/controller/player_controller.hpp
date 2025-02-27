#ifndef PIKA_PLAYER_CONTROLLER_HPP
#define PIKA_PLAYER_CONTROLLER_HPP

#include <pikaball/input.hpp>
#include <pikaball/physics/physics.hpp>

namespace pika {

/**
* Base class for all types of player controllers.
* A PlayerController must provide the player input based on the game state.
* The derived classes may be used to control the pikachus from a keyboard,
* a computer algorithm, data from a socket, etc.
*/
class PlayerController {
public:
  /**
   *
   * @param side the side of the field where this pikachu is playing
   */
  explicit PlayerController(const FieldSide& side) : field_side_(side) {}
  virtual ~PlayerController() = default;

  /**
   * Main controller function.
   * This function will be called by the game at each frame when the world is updated.
   * @param physics The current state of the game physics (Ball and players)
   * @return The player input for the player that is being controlled.
   */
  [[nodiscard]] virtual PlayerInput update(const PhysicsView& physics) = 0;

protected:
  FieldSide field_side_;
};

} // namespace pika

#endif // PIKA_PLAYER_CONTROLLER_HPP
