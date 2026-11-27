#include <doctest/doctest.h>

#include <pikaball/net/connection.hpp>
#include <pikaball/net/protocol.hpp>
#include <pikaball/net/udp_transport.hpp>

#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>

using namespace pika::net;

TEST_CASE("Protocol: messages round-trip") {
  CHECK(decode_type(encode_message(MessageType::Hello)) == MessageType::Hello);
  CHECK(decode_type(encode_message(MessageType::Bye)) == MessageType::Bye);

  const MatchSetup setup {.seed = 0xDEADBEEF, .input_delay = 3, .speed = 2, .points = 1};
  const auto welcome = encode_welcome(setup);
  CHECK(decode_type(welcome) == MessageType::Welcome);
  CHECK(decode_welcome(welcome) == setup);
}

TEST_CASE("Protocol: invalid datagrams are rejected") {
  std::vector<std::uint8_t> datagram = encode_welcome(MatchSetup {});

  SUBCASE("unrelated traffic") {
    datagram[0] = 0x50;
    CHECK_FALSE(decode_type(datagram));
  }
  SUBCASE("other protocol version") {
    datagram[0] = (protocol_id & 0xF0) | ((protocol_version + 1) & 0x0F);
    CHECK_FALSE(decode_type(datagram));
  }
  SUBCASE("unknown message type") {
    datagram[1] = 200;
    CHECK_FALSE(decode_type(datagram));
  }
  SUBCASE("truncated") {
    datagram.pop_back();
    CHECK(decode_type(datagram) == MessageType::Welcome);
    CHECK_FALSE(decode_welcome(datagram));
  }
  SUBCASE("too short for a header") {
    datagram.resize(1);
    CHECK_FALSE(decode_type(datagram));
  }
  SUBCASE("a Hello is not a Welcome") {
    CHECK_FALSE(decode_welcome(encode_message(MessageType::Hello)));
  }
}

namespace {

/**
 * Update the connections in rounds until done() is true (at most 100 rounds).
 * Fake clock: each round advances now_ms by 50 ms. A short sleep lets the loopback datagrams arrive.
 */
template <typename Done>
bool run_until(std::uint64_t& now_ms, const std::vector<Connection*>& connections, Done done) {
  for (int round = 0; round < 100; ++round) {
    for (Connection* connection : connections) {
      connection->update(now_ms);
    }
    if (done()) {
      return true;
    }
    now_ms += 50;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return false;
}

bool connected(const Connection& connection) {
  return connection.state() == ConnectionState::Connected;
}

} // namespace

TEST_CASE("Connection: handshake over loopback UDP") {
  const MatchSetup setup {.seed = 12345, .input_delay = 2, .speed = 1, .points = 2};
  Connection host = Connection::host(0, setup);  // Any free port
  Connection guest = Connection::join("127.0.0.1", host.local_port());
  CHECK(host.is_host());
  CHECK_FALSE(guest.is_host());
  CHECK(host.state() == ConnectionState::Waiting);
  CHECK(guest.state() == ConnectionState::Connecting);

  std::uint64_t now_ms {0};
  REQUIRE(run_until(now_ms, {&guest, &host}, [&] { return connected(host) && connected(guest); }));
  // The guest gets the host's setup, with the seed the host picked for this guest
  CHECK(guest.setup() == host.setup());
  CHECK(guest.setup().input_delay == setup.input_delay);
  CHECK(guest.setup().speed == setup.speed);
  CHECK(guest.setup().points == setup.points);

  SUBCASE("the keep-alive holds the connection") {
    // 10 s of rounds, longer than the timeout
    for (int round = 0; round < 200; ++round) {
      guest.update(now_ms);
      host.update(now_ms);
      now_ms += 50;
      std::this_thread::sleep_for(std::chrono::microseconds(200));
    }
    CHECK(connected(host));
    CHECK(connected(guest));
  }

  SUBCASE("the guest says Bye, and the host accepts a new guest") {
    guest.send_bye();
    REQUIRE(run_until(now_ms, {&host}, [&] { return host.state() == ConnectionState::Waiting; }));
    CHECK(host.peer_name().empty());

    Connection new_guest = Connection::join("127.0.0.1", host.local_port());
    REQUIRE(run_until(now_ms, {&new_guest, &host}, [&] { return connected(host) && connected(new_guest); }));
    CHECK(new_guest.setup() == host.setup());
    CHECK(host.peer_name() == "127.0.0.1:" + std::to_string(new_guest.local_port()));
  }

  SUBCASE("the host says Bye, and the guest is disconnected") {
    host.send_bye();
    REQUIRE(run_until(now_ms, {&guest}, [&] { return guest.state() == ConnectionState::Disconnected; }));
  }

  SUBCASE("silence: both sides time out") {
    // Let any datagram still in flight arrive, without advancing the clock
    for (int i = 0; i < 5; ++i) {
      guest.update(now_ms);
      host.update(now_ms);
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    now_ms += 6000;
    host.update(now_ms);
    guest.update(now_ms);
    CHECK(host.state() == ConnectionState::Waiting);
    CHECK(guest.state() == ConnectionState::Disconnected);
  }
}

TEST_CASE("Connection: the host only accepts a guest that sends a valid Hello") {
  Connection host = Connection::host(0, MatchSetup {});
  std::uint64_t now_ms {0};

  // A stranger sends garbage and a valid message that is not a Hello
  UdpTransport stranger = UdpTransport::connect("127.0.0.1", host.local_port());
  const std::vector<std::uint8_t> garbage {0x12, 0x34, 0x56};
  stranger.send(garbage);
  stranger.send(encode_message(MessageType::Bye));
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  host.update(now_ms);
  CHECK(host.state() == ConnectionState::Waiting);

  // The real guest still gets in
  Connection guest = Connection::join("127.0.0.1", host.local_port());
  REQUIRE(run_until(now_ms, {&guest, &host}, [&] { return connected(host) && connected(guest); }));
  const std::string guest_name = "127.0.0.1:" + std::to_string(guest.local_port());
  CHECK(host.peer_name() == guest_name);

  // Another Hello, or a Bye, from someone else doesn't replace or drop the guest
  stranger.send(encode_message(MessageType::Hello));
  stranger.send(encode_message(MessageType::Bye));
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  host.update(now_ms);
  CHECK(connected(host));
  CHECK(host.peer_name() == guest_name);
}
