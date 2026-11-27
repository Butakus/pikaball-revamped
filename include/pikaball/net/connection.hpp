#ifndef PIKA_NET_CONNECTION_HPP
#define PIKA_NET_CONNECTION_HPP

#include <cstdint>
#include <optional>
#include <string>

#include "protocol.hpp"
#include "udp_transport.hpp"

namespace pika::net {

/** How to start a network game (from the command line, for now) */
struct NetConfig {
  enum class Role { Host, Guest };
  Role role {Role::Host};
  std::string address;       // Host's address (guest only)
  std::uint16_t port {0};    // Port to listen on (host) or to send to (guest)
  std::uint8_t input_delay {2};
  FieldSide host_side {FieldSide::Left};  // Where the host plays (host only)
};

enum class ConnectionState {
  Waiting,       // Host: listening, no guest yet
  Connecting,    // Guest: asking the host to join
  Connected,     // Both machines have the match setup
  Disconnected,  // Guest: the host left or stopped answering. Final state.
};

/**
 * Connection between the two machines of a network game.
 *
 * Handshake: the guest sends Hello every hello interval until the host answers with a Welcome
 * holding the match setup. The host answers every Hello, so a lost Welcome is repaired by the next Hello.
 * Once connected, the guest keeps sending Hello at a slower pace, so both sides hear from each other
 * at least every keep-alive interval.
 *
 * Disconnection: a Bye, or no message for the timeout.
 * The host then goes back to Waiting and accepts a new guest. The guest goes to Disconnected.
 */
class Connection {
public:
  /**
   * Start hosting a game.
   * @param port UDP port to listen on (0 picks any free port)
   * @param setup The match setup sent to the guests
   */
  static Connection host(std::uint16_t port, const MatchSetup& setup);

  /**
   * Start joining the game hosted at address:port
   * @param address The IP address of the host
   * @param port The UDP port of the host
   */
  static Connection join(const std::string& address, std::uint16_t port);

  /**
   * Send and receive pending messages, and check the timeout. Call it once per frame.
   * @param now_ms Current time in milliseconds (any monotonic clock)
   */
  void update(std::uint64_t now_ms);

  /** Tell the peer that we are leaving (best effort, it may be lost). */
  void send_bye();

  [[nodiscard]] ConnectionState state() const { return state_; }
  /** The match setup. Only valid once connected (the guest receives it from the host). */
  [[nodiscard]] const MatchSetup& setup() const { return setup_; }
  [[nodiscard]] bool is_host() const { return is_host_; }
  /** @return The side where this machine's player plays. Only valid once connected. */
  [[nodiscard]] FieldSide local_side() const;
  /** @return The peer as "address:port" (empty if no peer) */
  [[nodiscard]] std::string peer_name() const;
  [[nodiscard]] std::uint16_t local_port() const { return transport_.local_port(); }

private:
  // Pace of the guest's Hello while connecting, and once connected (keep-alive)
  static constexpr std::uint64_t hello_interval_ms {100};
  static constexpr std::uint64_t keep_alive_interval_ms {1000};
  // Without any message from the peer for this long, it's gone
  static constexpr std::uint64_t timeout_ms {5000};

  UdpTransport transport_;
  bool is_host_;
  MatchSetup setup_ {};
  ConnectionState state_;
  // Time of the last Hello sent by the guest. Empty until the first one.
  std::optional<std::uint64_t> last_hello_ms_;
  // Time of the last valid message from the peer
  std::uint64_t last_received_ms_ {0};

  Connection(UdpTransport transport, bool is_host, const MatchSetup& setup);

  void handle_datagram(const Datagram& datagram, std::uint64_t now_ms);
  /** Bye or timeout: the host waits for a new guest, the guest is done */
  void disconnect();
};

} // namespace pika::net

#endif // PIKA_NET_CONNECTION_HPP
