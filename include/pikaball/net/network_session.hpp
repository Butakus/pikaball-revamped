#ifndef PIKA_NET_NETWORK_SESSION_HPP
#define PIKA_NET_NETWORK_SESSION_HPP

#include <cstdint>
#include <span>
#include <vector>

#include <pikaball/input.hpp>
#include <pikaball/physics/physics_common.hpp>

#include "protocol.hpp"

namespace pika::net {

/**
 * Input exchange of a network game, using deterministic lockstep.
 *
 * Both machines run the same simulation, and only exchange their players' inputs.
 * A tick (a simulated frame) can only run once both inputs for it are known, and both machines
 * then apply exactly the same TickInput, so the games stay identical.
 *
 * Input delay: the local input read at tick t is applied at tick t + input_delay (on both machines),
 * so it has input_delay frames to reach the other machine before it's needed. Ticks 0 to input_delay - 1
 * have idle inputs, known by both machines without any message.
 *
 * Messages: each Input message carries all the local inputs that the other machine hasn't acknowledged yet
 * (up to max_inputs_per_message), plus the acknowledgement of the inputs received from it.
 * A lost or reordered datagram is repaired by the next one, without resend timers.
 *
 * This class only holds the logic, it doesn't send anything: the caller moves the messages
 * (make_message / receive_message), so it can be tested without sockets.
 *
 * The inputs of the whole match are kept in memory: 2 bytes per tick and player (about 400 KB per hour).
 */
class NetworkSession {
public:
  /**
   * @param local_side Side of the player of this machine
   * @param input_delay Ticks between reading a local input and applying it (at least 1)
   */
  NetworkSession(FieldSide local_side, std::uint8_t input_delay);

  /**
   * Schedule the local input for tick current_tick() + input_delay.
   * Call it at least once per tick. Only the first call for a tick is scheduled: the power hit and pause
   * presses of later calls (e.g. while waiting for the other machine) are kept for the next tick, so they're not lost.
   */
  void add_local_input(const SideInput& input);

  /** @return True if both inputs for the current tick are known, so it can be simulated */
  [[nodiscard]] bool ready() const;

  /** @return The input for the current tick. Only valid if ready(). */
  [[nodiscard]] TickInput current_input() const;

  /** Move to the next tick, after simulating the current one. Only if ready(). */
  void advance();

  /** @return The tick to simulate next (0 at the start of the match) */
  [[nodiscard]] std::uint32_t current_tick() const { return current_tick_; }

  /** @return The Input message to send now: the unacknowledged local inputs and our ack */
  [[nodiscard]] std::vector<std::uint8_t> make_message() const;

  /**
   * Take the inputs and ack from an Input message of the other machine.
   * Duplicated, old and reordered messages are fine.
   * @return False if the datagram is not a valid Input message (it's ignored)
   */
  bool receive_message(std::span<const std::uint8_t> datagram);

private:
  FieldSide local_side_;
  std::uint8_t input_delay_;
  std::uint32_t current_tick_ {0};

  // Inputs of each player, indexed by tick. Always contiguous from tick 0.
  std::vector<SideInput> local_inputs_;
  std::vector<SideInput> remote_inputs_;
  // Number of local inputs the other machine has acknowledged (it holds all ticks below this one)
  std::uint32_t remote_received_ {0};
  // Power hit and pause presses that arrived after the input for the tick was scheduled
  SideInput carried_presses_ {};
};

} // namespace pika::net

#endif // PIKA_NET_NETWORK_SESSION_HPP
