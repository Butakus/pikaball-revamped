#ifndef PIKA_NET_PROTOCOL_HPP
#define PIKA_NET_PROTOCOL_HPP

#include <cstdint>
#include <optional>
#include <span>
#include <vector>

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
};

/** Everything both machines must agree on before the first frame. Chosen by the host. */
struct MatchSetup {
  std::uint32_t seed {0};        // Seed of the physics random numbers
  std::uint8_t input_delay {2};  // Frames between reading a local input and applying it
  std::uint8_t speed {0};        // Speed option (SpeedOptionSelection value)
  std::uint8_t points {0};       // Winning score option (PointsOptionSelection value)

  bool operator==(const MatchSetup&) const = default;
};

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

} // namespace pika::net

#endif // PIKA_NET_PROTOCOL_HPP
