#ifndef PIKA_COMPUTER_CONTROLLER_HPP
#define PIKA_COMPUTER_CONTROLLER_HPP

#include "player_controller.hpp"

namespace pika {

class ComputerController final : public PlayerController {
public:
  explicit ComputerController(const FieldSide& side);
  ~ComputerController() override = default;

  /**
   * Main controller function.
   * This function will be called by the game at each frame when the world is updated.
   * @param physics_view The current state of the game physics (Ball and players).
   * @return The player input for the player that is being controlled by the computer.
   */
  [[nodiscard]] PlayerInput on_update(const PhysicsView& physics_view) override;

  /**
   * This function will be called when a new round start.
   * It can be overridden if the controller wants to do any setup before the round starts.
   * @param physics The current state of the game physics (Ball and players).
   */
  void on_round_start(const PhysicsView& physics) override;

private:
  Ball ball_ {};
  Player player_ {FieldSide::Left};
  Player other_player_ {FieldSide::Right};

  // Integer value to compute distances differently if the player is on the right side
  const unsigned int is_player_right_;
  // Left and right bounds for our field side
  const unsigned int left_bound_;
  const unsigned int right_bound_;

  /**
   * It flips randomly to 0 or 1 on each update (FUN_00402360)
   * when ball is hanging around on the other player's side.
   * If it is 0, computer player stands by around the middle point of their side.
   * If it is 1, computer player stands by adjacent to the net.
   */
  int computer_idle_position_ {0};  // 0xDC
  /**
   * This value is initialized to (_rand() % 5) before the start of every round.
   * The greater the number, the bolder the computer player.
   *
   * If computer has higher boldness,
   * judges more the ball is hanging around the other player's side,
   * has greater distance to the expected landing point of the ball,
   * jumps more, and dives less.
   * See comments inside decide_input_power_hit function (FUN_00402360).
   *
   * Possible values: 0, 1, 2, 3 or 4
   */
  int boldness_ {0};  // 0xD8

  /**
   * Check if the player should power hit and set the hit direction.
   * The input parameter will be modified with the direction.
   * FUN_00402630, decideWhetherInputPowerHit
   * @param physics_view The current state of the game physics (Ball and players).
   * @param input Input for the player controlled by the computer.
                  The input directions will be modified.
   * @return True is the computer decides to power hit
   */
  bool decide_input_power_hit(const PhysicsView& physics_view, PlayerInput& input) const;
};

} // namespace pika

#endif // PIKA_COMPUTER_CONTROLLER_HPP
