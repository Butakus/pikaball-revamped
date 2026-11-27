#include <pikaball/net/udp_transport.hpp>

#include <system_error>

#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>

namespace pika::net {

namespace {

using asio::ip::udp;

/** Larger than any message of the protocol. Longer datagrams are truncated and rejected by the decoder. */
constexpr std::size_t max_datagram_size {1024};

PeerAddress to_peer_address(const udp::endpoint& endpoint) {
  return {.ip = endpoint.address().to_v4().to_bytes(), .port = endpoint.port()};
}

udp::endpoint to_endpoint(const PeerAddress& peer) {
  return {asio::ip::address_v4(peer.ip), peer.port};
}

} // namespace

std::string PeerAddress::to_string() const {
  return to_endpoint(*this).address().to_string() + ":" + std::to_string(port);
}

struct UdpTransport::Impl {
  // The socket needs an io_context, but it is never run: all operations are synchronous and non-blocking
  asio::io_context io_context;
  udp::socket socket {io_context};
  std::array<std::uint8_t, max_datagram_size> buffer {};
};

UdpTransport::UdpTransport(std::unique_ptr<Impl> impl, std::optional<PeerAddress> peer) :
  impl_(std::move(impl)),
  peer_(peer)
{}
UdpTransport::~UdpTransport() = default;
UdpTransport::UdpTransport(UdpTransport&&) noexcept = default;
UdpTransport& UdpTransport::operator=(UdpTransport&&) noexcept = default;

UdpTransport UdpTransport::listen(const std::uint16_t port) {
  auto impl = std::make_unique<Impl>();
  impl->socket.open(udp::v4());
  impl->socket.bind(udp::endpoint(udp::v4(), port));
  impl->socket.non_blocking(true);
  return UdpTransport(std::move(impl));
}

UdpTransport UdpTransport::connect(const std::string& address, const std::uint16_t port) {
  auto impl = std::make_unique<Impl>();
  udp::resolver resolver {impl->io_context};
  // Take the first IPv4 address. Throws if the name can't be resolved.
  const udp::endpoint host = *resolver.resolve(udp::v4(), address, std::to_string(port)).begin();
  impl->socket.open(udp::v4());
  impl->socket.non_blocking(true);
  return UdpTransport(std::move(impl), to_peer_address(host));
}

void UdpTransport::send(const std::span<const std::uint8_t> datagram) {
  if (!peer_) {
    return;
  }
  // UDP is unreliable anyway: a datagram that can't be sent right now is just lost
  std::error_code error;
  impl_->socket.send_to(asio::buffer(datagram.data(), datagram.size()), to_endpoint(*peer_), 0, error);
}

std::optional<Datagram> UdpTransport::receive() {
  while (true) {
    udp::endpoint sender;
    std::error_code error;
    const std::size_t size = impl_->socket.receive_from(asio::buffer(impl_->buffer), sender, 0, error);
    if (error == asio::error::connection_reset || error == asio::error::connection_refused) {
      // Windows reports an ICMP "port unreachable" from an earlier send here.
      // The peer may not be listening yet, so keep reading.
      continue;
    }
    if (error) {
      return std::nullopt;  // Nothing waiting (would_block), or any other error: try again next frame
    }
    return Datagram {
      .data = {impl_->buffer.begin(), impl_->buffer.begin() + static_cast<std::ptrdiff_t>(size)},
      .sender = to_peer_address(sender),
    };
  }
}

std::uint16_t UdpTransport::local_port() const {
  return impl_->socket.local_endpoint().port();
}

} // namespace pika::net
