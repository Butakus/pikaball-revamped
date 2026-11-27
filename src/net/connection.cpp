#include <pikaball/net/connection.hpp>

#include <random>
#include <utility>

namespace pika::net {

Connection::Connection(UdpTransport transport, const bool is_host, const MatchSetup& setup) :
  transport_(std::move(transport)),
  is_host_(is_host),
  setup_(setup),
  state_(is_host ? ConnectionState::Waiting : ConnectionState::Connecting)
{}

Connection Connection::host(const std::uint16_t port, const MatchSetup& setup) {
  return {UdpTransport::listen(port), true, setup};
}

Connection Connection::join(const std::string& address, const std::uint16_t port) {
  return {UdpTransport::connect(address, port), false, MatchSetup {}};
}

void Connection::update(const std::uint64_t now_ms) {
  while (const auto datagram = transport_.receive()) {
    handle_datagram(*datagram, now_ms);
  }

  if (state_ == ConnectionState::Connected && now_ms - last_received_ms_ >= timeout_ms) {
    disconnect();
  }

  // The guest asks to join until the host answers, then keeps the connection alive
  if (!is_host_ && (state_ == ConnectionState::Connecting || state_ == ConnectionState::Connected)) {
    const std::uint64_t interval =
      (state_ == ConnectionState::Connecting) ? hello_interval_ms : keep_alive_interval_ms;
    if (!last_hello_ms_ || now_ms - *last_hello_ms_ >= interval) {
      transport_.send(encode_message(MessageType::Hello));
      last_hello_ms_ = now_ms;
    }
  }
}

void Connection::send_bye() {
  if (state_ == ConnectionState::Connected) {
    transport_.send(encode_message(MessageType::Bye));
  }
}

std::string Connection::peer_name() const {
  return transport_.peer() ? transport_.peer()->to_string() : std::string {};
}

void Connection::handle_datagram(const Datagram& datagram, const std::uint64_t now_ms) {
  const auto type = decode_type(datagram.data);
  if (!type) {
    return;  // Not ours, or another protocol version
  }

  // A waiting host accepts the first machine that sends a valid Hello as its guest
  if (state_ == ConnectionState::Waiting && *type == MessageType::Hello) {
    transport_.set_peer(datagram.sender);
    setup_.seed = std::random_device{}();  // A new match, a new seed
    state_ = ConnectionState::Connected;
  }
  // Ignore everyone else
  if (transport_.peer() != datagram.sender) {
    return;
  }
  last_received_ms_ = now_ms;

  switch (*type) {
  case MessageType::Hello:
    if (is_host_) {
      // Answer every Hello: if a Welcome is lost, the guest asks again
      transport_.send(encode_welcome(setup_));
    }
    break;
  case MessageType::Welcome:
    if (!is_host_ && state_ == ConnectionState::Connecting) {
      if (const auto setup = decode_welcome(datagram.data)) {
        setup_ = *setup;
        state_ = ConnectionState::Connected;
      }
    }
    break;
  case MessageType::Bye:
    if (state_ == ConnectionState::Connected) {
      disconnect();
    }
    break;
  }
}

void Connection::disconnect() {
  if (is_host_) {
    transport_.clear_peer();
    state_ = ConnectionState::Waiting;
  } else {
    state_ = ConnectionState::Disconnected;
  }
}

} // namespace pika::net
