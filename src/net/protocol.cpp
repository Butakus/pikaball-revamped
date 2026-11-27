#include <pikaball/net/protocol.hpp>

#include <utility>

namespace pika::net {

namespace {

constexpr std::size_t header_size {2};
constexpr std::size_t welcome_size {header_size + 4 + 3};

std::vector<std::uint8_t> make_header(const MessageType type) {
  return {protocol_id, std::to_underlying(type)};
}

void write_u32(std::vector<std::uint8_t>& out, const std::uint32_t value) {
  for (int byte = 0; byte < 4; ++byte) {
    out.push_back(static_cast<std::uint8_t>(value >> (8 * byte)));
  }
}

std::uint32_t read_u32(const std::span<const std::uint8_t> in, const std::size_t offset) {
  std::uint32_t value {0};
  for (int byte = 0; byte < 4; ++byte) {
    value |= static_cast<std::uint32_t>(in[offset + byte]) << (8 * byte);
  }
  return value;
}

} // namespace

std::vector<std::uint8_t> encode_message(const MessageType type) {
  return make_header(type);
}

std::vector<std::uint8_t> encode_welcome(const MatchSetup& setup) {
  std::vector<std::uint8_t> out = make_header(MessageType::Welcome);
  write_u32(out, setup.seed);
  out.push_back(setup.input_delay);
  out.push_back(setup.speed);
  out.push_back(setup.points);
  return out;
}

std::optional<MessageType> decode_type(const std::span<const std::uint8_t> datagram) {
  if (datagram.size() < header_size || datagram[0] != protocol_id) {
    return std::nullopt;
  }
  switch (const auto type = static_cast<MessageType>(datagram[1])) {
    case MessageType::Hello:
    case MessageType::Welcome:
    case MessageType::Bye:
      return type;
  }
  return std::nullopt;
}

std::optional<MatchSetup> decode_welcome(const std::span<const std::uint8_t> datagram) {
  if (decode_type(datagram) != MessageType::Welcome || datagram.size() != welcome_size) {
    return std::nullopt;
  }
  return MatchSetup {
    .seed = read_u32(datagram, header_size),
    .input_delay = datagram[header_size + 4],
    .speed = datagram[header_size + 5],
    .points = datagram[header_size + 6],
  };
}

} // namespace pika::net
