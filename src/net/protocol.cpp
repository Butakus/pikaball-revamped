#include <pikaball/net/protocol.hpp>

#include <utility>

namespace pika::net {

namespace {

constexpr std::size_t header_size {2};
constexpr std::size_t welcome_size {header_size + 4 + 4};
constexpr std::size_t input_header_size {header_size + 4 + 4 + 1};

std::vector<std::uint8_t> make_header(const MessageType type) {
  return {protocol_id, std::to_underlying(type)};
}

void write_u32(std::vector<std::uint8_t>& out, const std::uint32_t value) {
  for (int byte = 0; byte < 4; ++byte) {
    out.push_back(static_cast<std::uint8_t>(value >> (8 * byte)));
  }
}

void write_u16(std::vector<std::uint8_t>& out, const std::uint16_t value) {
  out.push_back(static_cast<std::uint8_t>(value));
  out.push_back(static_cast<std::uint8_t>(value >> 8));
}

std::uint32_t read_u32(const std::span<const std::uint8_t> in, const std::size_t offset) {
  std::uint32_t value {0};
  for (int byte = 0; byte < 4; ++byte) {
    value |= static_cast<std::uint32_t>(in[offset + byte]) << (8 * byte);
  }
  return value;
}

std::uint16_t read_u16(const std::span<const std::uint8_t> in, const std::size_t offset) {
  return static_cast<std::uint16_t>(in[offset] | (in[offset + 1] << 8));
}

/** Direction as 2 bits: 0 none, 1 negative (left / up), 2 positive (right / down) */
template <typename Dir>
std::uint16_t encode_direction(const Dir direction) {
  switch (std::to_underlying(direction)) {
  case -1: return 1;
  case 1: return 2;
  default: return 0;
  }
}

template <typename Dir>
std::optional<Dir> decode_direction(const std::uint16_t bits) {
  switch (bits) {
  case 0: return static_cast<Dir>(0);
  case 1: return static_cast<Dir>(-1);
  case 2: return static_cast<Dir>(1);
  default: return std::nullopt;
  }
}

constexpr std::uint16_t power_hit_bit {1u << 4};
constexpr std::uint16_t pause_bit {1u << 5};
constexpr std::uint16_t reserved_bits {0xFFC0};

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
  out.push_back(setup.host_side == FieldSide::Left ? 0 : 1);
  return out;
}

std::uint16_t encode_side_input(const SideInput& input) {
  std::uint16_t bits = encode_direction(input.player.direction_x);
  bits |= static_cast<std::uint16_t>(encode_direction(input.player.direction_y) << 2);
  if (input.player.power_hit) {
    bits |= power_hit_bit;
  }
  if (input.pause_toggle) {
    bits |= pause_bit;
  }
  return bits;
}

std::optional<SideInput> decode_side_input(const std::uint16_t bits) {
  const auto direction_x = decode_direction<DirX>(bits & 0x3u);
  const auto direction_y = decode_direction<DirY>((bits >> 2) & 0x3u);
  if (!direction_x || !direction_y || (bits & reserved_bits) != 0) {
    return std::nullopt;
  }
  return SideInput {
    .player = {
      .direction_x = *direction_x,
      .direction_y = *direction_y,
      .power_hit = (bits & power_hit_bit) != 0,
    },
    .pause_toggle = (bits & pause_bit) != 0,
  };
}

std::optional<MessageType> decode_type(const std::span<const std::uint8_t> datagram) {
  if (datagram.size() < header_size || datagram[0] != protocol_id) {
    return std::nullopt;
  }
  switch (const auto type = static_cast<MessageType>(datagram[1])) {
    case MessageType::Hello:
    case MessageType::Welcome:
    case MessageType::Bye:
    case MessageType::Input:
      return type;
  }
  return std::nullopt;
}

std::optional<MatchSetup> decode_welcome(const std::span<const std::uint8_t> datagram) {
  if (decode_type(datagram) != MessageType::Welcome || datagram.size() != welcome_size) {
    return std::nullopt;
  }
  const std::uint8_t host_side = datagram[header_size + 7];
  if (host_side > 1) {
    return std::nullopt;
  }
  return MatchSetup {
    .seed = read_u32(datagram, header_size),
    .input_delay = datagram[header_size + 4],
    .speed = datagram[header_size + 5],
    .points = datagram[header_size + 6],
    .host_side = host_side == 0 ? FieldSide::Left : FieldSide::Right,
  };
}

std::vector<std::uint8_t> encode_input(const InputMessage& message) {
  std::vector<std::uint8_t> out = make_header(MessageType::Input);
  out.reserve(input_header_size + 2 * message.inputs.size());
  write_u32(out, message.received);
  write_u32(out, message.first_tick);
  out.push_back(static_cast<std::uint8_t>(message.inputs.size()));
  for (const SideInput& input : message.inputs) {
    write_u16(out, encode_side_input(input));
  }
  return out;
}

std::optional<InputMessage> decode_input(const std::span<const std::uint8_t> datagram) {
  if (decode_type(datagram) != MessageType::Input || datagram.size() < input_header_size) {
    return std::nullopt;
  }
  const std::size_t count = datagram[input_header_size - 1];
  if (count > max_inputs_per_message || datagram.size() != input_header_size + 2 * count) {
    return std::nullopt;
  }
  InputMessage message {
    .received = read_u32(datagram, header_size),
    .first_tick = read_u32(datagram, header_size + 4),
    .inputs = {},
  };
  message.inputs.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    const auto input = decode_side_input(read_u16(datagram, input_header_size + 2 * i));
    if (!input) {
      return std::nullopt;
    }
    message.inputs.push_back(*input);
  }
  return message;
}

} // namespace pika::net
