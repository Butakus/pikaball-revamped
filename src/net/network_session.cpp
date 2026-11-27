#include <pikaball/net/network_session.hpp>

#include <algorithm>

namespace pika::net {

NetworkSession::NetworkSession(const FieldSide local_side, const std::uint8_t input_delay) :
  local_side_(local_side),
  input_delay_(std::max<std::uint8_t>(input_delay, 1)),
  // The first input_delay ticks are idle for both players, and both machines know it
  local_inputs_(input_delay_),
  remote_inputs_(input_delay_),
  remote_received_(input_delay_)
{}

void NetworkSession::add_local_input(const SideInput& input) {
  if (local_inputs_.size() > current_tick_ + input_delay_) {
    // The input for this tick is already scheduled: keep the presses for the next one
    carried_presses_.player.power_hit |= input.player.power_hit;
    carried_presses_.pause_toggle |= input.pause_toggle;
    return;
  }
  SideInput scheduled = input;
  scheduled.player.power_hit |= carried_presses_.player.power_hit;
  scheduled.pause_toggle |= carried_presses_.pause_toggle;
  carried_presses_ = {};
  local_inputs_.push_back(scheduled);
}

bool NetworkSession::ready() const {
  return current_tick_ < local_inputs_.size() && current_tick_ < remote_inputs_.size();
}

TickInput NetworkSession::current_input() const {
  const SideInput& local = local_inputs_[current_tick_];
  const SideInput& remote = remote_inputs_[current_tick_];
  const bool local_is_left = local_side_ == FieldSide::Left;
  return {
    .left = local_is_left ? local.player : remote.player,
    .right = local_is_left ? remote.player : local.player,
    .pause_toggle = local.pause_toggle || remote.pause_toggle,
  };
}

void NetworkSession::advance() {
  if (ready()) {
    ++current_tick_;
  }
}

std::vector<std::uint8_t> NetworkSession::make_message() const {
  // Send the oldest unacknowledged inputs first: the other machine needs them before the newer ones
  const std::size_t first = std::min<std::size_t>(remote_received_, local_inputs_.size());
  const std::size_t count = std::min(local_inputs_.size() - first, max_inputs_per_message);
  const auto window_begin = local_inputs_.begin() + static_cast<std::ptrdiff_t>(first);
  return encode_input({
    .received = static_cast<std::uint32_t>(remote_inputs_.size()),
    .first_tick = static_cast<std::uint32_t>(first),
    .inputs = {window_begin, window_begin + static_cast<std::ptrdiff_t>(count)},
  });
}

bool NetworkSession::receive_message(const std::span<const std::uint8_t> datagram) {
  const auto message = decode_input(datagram);
  if (!message) {
    return false;
  }
  // The ack only grows (an old, reordered message can carry a smaller one).
  // It can't be larger than the inputs we have sent.
  remote_received_ = std::clamp<std::uint32_t>(
    message->received, remote_received_, static_cast<std::uint32_t>(local_inputs_.size()));

  // Append the inputs that continue our list. Older ones are duplicates.
  // A gap can't happen with a well-behaved peer (it sends from our ack), so ignore it.
  for (std::size_t i = 0; i < message->inputs.size(); ++i) {
    const std::uint64_t tick = static_cast<std::uint64_t>(message->first_tick) + i;
    if (tick == remote_inputs_.size()) {
      remote_inputs_.push_back(message->inputs[i]);
    }
  }
  return true;
}

} // namespace pika::net
