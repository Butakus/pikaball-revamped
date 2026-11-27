#ifndef PIKA_NET_PROTOCOL_HPP
#define PIKA_NET_PROTOCOL_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

#include <pikaball/input.hpp>
#include <pikaball/physics/physics_common.hpp>

namespace pika::net {

/*
 * Wire format of the network messages. Every datagram starts with a 2-byte header:
 * protocol id and message type. Multi-byte values are little-endian.
 */

/** Peers with a different version cannot play together */
constexpr std::uint8_t protocol_version {1};
/**
 * First byte of every datagram: a fixed pattern in the high 4 bits and the version in the low 4 bits.
 * Discards unrelated traffic and peers with another version.
 */
constexpr std::uint8_t protocol_id {0xB0 | protocol_version};
static_assert(protocol_version <= 0x0F, "The protocol version must fit in 4 bits");

enum class MessageType : std::uint8_t {
  Hello = 1,    // Guest -> host: asks to join (and keeps the connection alive)
  Welcome = 2,  // Host -> guest: accepts and sends the match setup (answers every Hello)
  Bye = 3,      // Either way: leaving the game
  Input = 4,    // Either way: the sender's inputs that the receiver hasn't confirmed yet
};

/** Everything both machines must agree on before the first frame. Chosen by the host. */
struct MatchSetup {
  std::uint32_t seed {0};        // Seed of the physics random numbers
  std::uint8_t input_delay {2};  // Frames between reading a local input and applying it
  std::uint8_t speed {0};        // Speed option (SpeedOptionSelection value)
  std::uint8_t points {0};       // Winning score option (PointsOptionSelection value)
  FieldSide host_side {FieldSide::Left};  // Where the host plays. The guest plays on the other side.

  bool operator==(const MatchSetup&) const = default;
};

/**
 * Input message: a window of consecutive inputs of the sender's player, plus an ack.
 * Layout after the header: u32 received, u32 first_tick, u8 count, count x u16 inputs.
 */
struct InputMessage {
  // Number of the receiver's inputs the sender holds (all ticks below this one): the ack
  std::uint32_t received {0};
  // Tick of the first input in the window
  std::uint32_t first_tick {0};
  // Inputs for first_tick, first_tick + 1, ...
  std::vector<SideInput> inputs;

  bool operator==(const InputMessage&) const = default;
};

/** Most inputs in one message. The window of unacknowledged inputs is cut to this size. */
constexpr std::size_t max_inputs_per_message {64};

/**
 * Pack a SideInput in 16 bits:
 * bits 0-1 x direction, bits 2-3 y direction (0 none, 1 negative, 2 positive), bit 4 power hit, bit 5 pause.
 * Bits 6-15 are reserved (0).
 */
std::uint16_t encode_side_input(const SideInput& input);

/** @return The SideInput, or nothing if the bits are not a valid input */
std::optional<SideInput> decode_side_input(std::uint16_t bits);

/** Build a message that has no content after the header (Hello, Bye) */
std::vector<std::uint8_t> encode_message(MessageType type);

std::vector<std::uint8_t> encode_welcome(const MatchSetup& setup);

/**
 * Check the header of a datagram.
 * @return The message type, or nothing if the datagram is not a valid message of this protocol version
 */
std::optional<MessageType> decode_type(std::span<const std::uint8_t> datagram);

/** @return The match setup, or nothing if the datagram is not a valid Welcome message */
std::optional<MatchSetup> decode_welcome(std::span<const std::uint8_t> datagram);

/** @param message At most max_inputs_per_message inputs */
std::vector<std::uint8_t> encode_input(const InputMessage& message);

/** @return The input message, or nothing if the datagram is not a valid Input message */
std::optional<InputMessage> decode_input(std::span<const std::uint8_t> datagram);

} // namespace pika::net

#endif // PIKA_NET_PROTOCOL_HPP
