#ifndef PIKA_NET_UDP_TRANSPORT_HPP
#define PIKA_NET_UDP_TRANSPORT_HPP

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace pika::net {

/** IPv4 address and UDP port of a machine */
struct PeerAddress {
  std::array<std::uint8_t, 4> ip {};
  std::uint16_t port {0};

  bool operator==(const PeerAddress&) const = default;

  /** @return "a.b.c.d:port", for logging */
  [[nodiscard]] std::string to_string() const;
};

/** A datagram and the machine that sent it */
struct Datagram {
  std::vector<std::uint8_t> data;
  PeerAddress sender;
};

/**
 * A non-blocking IPv4 UDP socket.
 * It never waits: the game polls it once per frame from the main thread, so no threads are needed.
 *
 * It sends to a single peer. The guest knows the host's address from the start.
 * The host has no peer until its Connection accepts a guest (set_peer).
 * The transport receives from anyone: choosing which senders to listen to is the Connection's job.
 *
 * Asio errors (e.g. the port is in use, the address can't be resolved) are thrown as std::system_error.
 */
class UdpTransport {
public:
  /**
   * Host side: listen for guests.
   * @param port UDP port to listen on (0 picks any free port, see local_port())
   */
  static UdpTransport listen(std::uint16_t port);

  /**
   * Guest side: talk to the host.
   * @param address IPv4 address or host name of the host
   * @param port UDP port of the host
   */
  static UdpTransport connect(const std::string& address, std::uint16_t port);

  ~UdpTransport();
  UdpTransport(UdpTransport&&) noexcept;
  UdpTransport& operator=(UdpTransport&&) noexcept;
  UdpTransport(const UdpTransport&) = delete;
  UdpTransport& operator=(const UdpTransport&) = delete;

  /** Send a datagram to the peer. Does nothing while there is no peer. */
  void send(std::span<const std::uint8_t> datagram);

  /** @return The next datagram received from any sender, or nothing if none is waiting */
  std::optional<Datagram> receive();

  void set_peer(const PeerAddress& peer) { peer_ = peer; }
  void clear_peer() { peer_.reset(); }
  [[nodiscard]] const std::optional<PeerAddress>& peer() const { return peer_; }

  [[nodiscard]] std::uint16_t local_port() const;

private:
  // Pimpl, so the Asio headers are only included by udp_transport.cpp
  struct Impl;
  std::unique_ptr<Impl> impl_;
  std::optional<PeerAddress> peer_;

  explicit UdpTransport(std::unique_ptr<Impl> impl, std::optional<PeerAddress> peer = std::nullopt);
};

} // namespace pika::net

#endif // PIKA_NET_UDP_TRANSPORT_HPP
