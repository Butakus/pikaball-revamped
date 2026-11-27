#include <doctest/doctest.h>

#include <pikaball/net/network_session.hpp>
#include <pikaball/net/protocol.hpp>
#include <pikaball/physics/physics.hpp>
#include <pikaball/random.hpp>

#include <cstdint>
#include <vector>

#include "helpers.hpp"

using namespace pika;
using namespace pika::net;
using namespace pika::test;

namespace {

/** Random but valid side input. Pause presses are rare. */
SideInput random_side_input(Rng& rng) {
  return {
    .player = {
      .direction_x = static_cast<DirX>(rng.rand_int() % 3 - 1),
      .direction_y = static_cast<DirY>(rng.rand_int() % 3 - 1),
      .power_hit = rng.rand_int() % 4 == 0,
    },
    .pause_toggle = rng.rand_int() % 200 == 0,
  };
}

SideInput hit() {
  return {.player = input::power_hit(), .pause_toggle = false};
}

/** Decode the message a session would send now */
InputMessage message_of(const NetworkSession& session) {
  const auto message = decode_input(session.make_message());
  REQUIRE(message);
  return *message;
}

} // namespace

TEST_CASE("Protocol: side inputs round-trip in 16 bits") {
  for (const DirX x : {DirX::Left, DirX::None, DirX::Right}) {
    for (const DirY y : {DirY::Up, DirY::None, DirY::Down}) {
      for (const bool power_hit : {false, true}) {
        for (const bool pause : {false, true}) {
          const SideInput input {.player = {x, y, power_hit}, .pause_toggle = pause};
          CHECK(decode_side_input(encode_side_input(input)) == input);
        }
      }
    }
  }
  CHECK_FALSE(decode_side_input(0x0003));  // x direction 3 doesn't exist
  CHECK_FALSE(decode_side_input(0x000C));  // y direction 3 doesn't exist
  CHECK_FALSE(decode_side_input(0x0040));  // Reserved bit
}

TEST_CASE("Protocol: input messages round-trip and invalid ones are rejected") {
  const InputMessage message {
    .received = 123456,
    .first_tick = 70000,
    .inputs = {SideInput {}, hit(), SideInput {.player = input::move(DirX::Left), .pause_toggle = true}},
  };
  std::vector<std::uint8_t> datagram = encode_input(message);
  CHECK(decode_input(datagram) == message);

  SUBCASE("empty window") {
    const InputMessage ack_only {.received = 5, .first_tick = 9, .inputs = {}};
    CHECK(decode_input(encode_input(ack_only)) == ack_only);
  }
  SUBCASE("truncated") {
    datagram.pop_back();
    CHECK_FALSE(decode_input(datagram));
  }
  SUBCASE("extra bytes") {
    datagram.push_back(0);
    CHECK_FALSE(decode_input(datagram));
  }
  SUBCASE("an invalid input") {
    datagram.back() = 0xFF;
    CHECK_FALSE(decode_input(datagram));
  }
  SUBCASE("more inputs than allowed") {
    datagram[10] = static_cast<std::uint8_t>(max_inputs_per_message + 1);
    datagram.resize(11 + 2 * (max_inputs_per_message + 1), 0);
    CHECK_FALSE(decode_input(datagram));
  }
}

TEST_CASE("NetworkSession: the first ticks are idle and need no message") {
  constexpr std::uint8_t delay = 3;
  NetworkSession session(FieldSide::Left, delay);
  for (std::uint32_t tick = 0; tick < delay; ++tick) {
    session.add_local_input(hit());
    REQUIRE(session.ready());
    CHECK(session.current_input().left == input::idle());
    CHECK(session.current_input().right == input::idle());
    session.advance();
  }
  // From here on, the remote input is needed
  session.add_local_input(hit());
  CHECK(session.current_tick() == delay);
  CHECK_FALSE(session.ready());
  session.advance();  // Does nothing while not ready
  CHECK(session.current_tick() == delay);
}

TEST_CASE("NetworkSession: inputs are applied input_delay ticks later, on the right side") {
  constexpr std::uint8_t delay = 2;
  NetworkSession host(FieldSide::Left, delay);
  NetworkSession guest(FieldSide::Right, delay);

  // Tick 0: the host presses hit, the guest moves left. Both are scheduled for tick 2.
  host.add_local_input(hit());
  guest.add_local_input({.player = input::move(DirX::Left), .pause_toggle = true});
  for (int tick = 0; tick < delay; ++tick) {
    REQUIRE(host.ready());
    REQUIRE(guest.ready());
    host.advance();
    guest.advance();
    host.add_local_input({});
    guest.add_local_input({});
  }
  CHECK_FALSE(host.ready());
  REQUIRE(host.receive_message(guest.make_message()));
  REQUIRE(guest.receive_message(host.make_message()));
  REQUIRE(host.ready());
  REQUIRE(guest.ready());

  const TickInput expected {.left = input::power_hit(), .right = input::move(DirX::Left), .pause_toggle = true};
  CHECK(host.current_input().left == expected.left);
  CHECK(host.current_input().right == expected.right);
  CHECK(host.current_input().pause_toggle);
  CHECK(guest.current_input().left == expected.left);
  CHECK(guest.current_input().right == expected.right);
  CHECK(guest.current_input().pause_toggle);
}

TEST_CASE("NetworkSession: presses while waiting are carried to the next tick") {
  NetworkSession session(FieldSide::Left, 1);
  session.add_local_input({});        // Scheduled for tick 1
  session.add_local_input(hit());     // Too late for tick 1: carried
  session.advance();                  // Tick 0 is idle and known
  CHECK(session.current_tick() == 1);
  session.add_local_input({});        // Scheduled for tick 2, with the carried hit

  const InputMessage message = message_of(session);
  REQUIRE(message.inputs.size() == 2);  // Ticks 1 and 2 (tick 0 is known by both)
  CHECK(message.first_tick == 1);
  CHECK_FALSE(message.inputs[0].player.power_hit);
  CHECK(message.inputs[1].player.power_hit);
}

TEST_CASE("NetworkSession: the window shrinks once the other machine acknowledges") {
  constexpr std::uint8_t delay = 2;
  NetworkSession host(FieldSide::Left, delay);
  NetworkSession guest(FieldSide::Right, delay);
  host.add_local_input(hit());

  // Before any acknowledgement, the window holds every input after the idle ones
  InputMessage message = message_of(host);
  CHECK(message.first_tick == delay);
  CHECK(message.inputs.size() == 1);
  CHECK(message.received == delay);

  // The guest receives it and acknowledges
  REQUIRE(guest.receive_message(host.make_message()));
  CHECK(message_of(guest).received == delay + 1);
  REQUIRE(host.receive_message(guest.make_message()));
  message = message_of(host);
  CHECK(message.first_tick == delay + 1);
  CHECK(message.inputs.empty());

  // An old message with a smaller acknowledgement doesn't move it back
  REQUIRE(host.receive_message(encode_input({.received = 0, .first_tick = 0, .inputs = {}})));
  CHECK(message_of(host).first_tick == delay + 1);
}

namespace {

/** One direction of a bad network: drops, delays (so reorders) and duplicates datagrams */
class FakeLink {
public:
  explicit FakeLink(const std::uint32_t seed) : rng_(seed) {}

  void send(const std::uint64_t now, const std::vector<std::uint8_t>& datagram) {
    if (rng_.rand_int() % 100 < loss_percent) {
      return;
    }
    in_flight_.push_back({now + rng_.rand_int() % max_delay, datagram});
    if (rng_.rand_int() % 100 < duplicate_percent) {
      in_flight_.push_back({now + rng_.rand_int() % max_delay, datagram});
    }
  }

  /** @return The datagrams that arrive at this time, in random order */
  std::vector<std::vector<std::uint8_t>> deliver(const std::uint64_t now) {
    std::vector<std::vector<std::uint8_t>> arrived;
    std::erase_if(in_flight_, [&](const InFlight& datagram) {
      if (datagram.arrival <= now) {
        arrived.push_back(datagram.data);
        return true;
      }
      return false;
    });
    return arrived;
  }

private:
  static constexpr int loss_percent {30};
  static constexpr int duplicate_percent {10};
  static constexpr int max_delay {5};

  struct InFlight {
    std::uint64_t arrival;
    std::vector<std::uint8_t> data;
  };
  Rng rng_;
  std::vector<InFlight> in_flight_;
};

/** One machine: its session, its own copy of the game, and what it simulated */
struct Machine {
  NetworkSession session;
  Physics physics;
  Rng input_rng;
  Rng speed_rng;
  std::vector<TickInput> applied;
  std::vector<PhysicsView> states;

  Machine(const FieldSide side, const std::uint32_t seed, const std::uint32_t input_seed) :
    session(side, 2), physics(seed), input_rng(input_seed), speed_rng(input_seed + 1)
  {
    physics.init_round(FieldSide::Left);
  }

  /** A frame of the game loop: read the local input, simulate the tick if possible, send a message */
  void frame(const std::uint64_t now, FakeLink& outgoing, const int skip_percent) {
    // A slower machine sometimes misses frames
    if (speed_rng.rand_int() % 100 < skip_percent) {
      return;
    }
    session.add_local_input(random_side_input(input_rng));
    if (session.ready()) {
      const TickInput tick = session.current_input();
      if (physics.update(tick.left, tick.right)) {
        physics.init_round(physics.ball().x() < ground_h_width ? FieldSide::Right : FieldSide::Left);
      }
      applied.push_back(tick);
      states.emplace_back(physics);
      session.advance();
    }
    outgoing.send(now, session.make_message());
  }

  void receive(const std::uint64_t now, FakeLink& incoming) {
    for (const auto& datagram : incoming.deliver(now)) {
      REQUIRE(session.receive_message(datagram));
    }
  }
};

} // namespace

TEST_CASE("NetworkSession: two machines stay in sync over a bad network") {
  constexpr std::uint32_t seed = 2024;
  constexpr std::size_t ticks = 3000;
  Machine host(FieldSide::Left, seed, 1);
  Machine guest(FieldSide::Right, seed, 2);
  FakeLink to_guest(10);
  FakeLink to_host(11);

  std::uint64_t now = 0;
  while ((host.applied.size() < ticks || guest.applied.size() < ticks) && now < 100 * ticks) {
    host.frame(now, to_guest, 0);
    guest.frame(now, to_host, 25);
    ++now;
    host.receive(now, to_host);
    guest.receive(now, to_guest);
  }
  REQUIRE(host.applied.size() >= ticks);
  REQUIRE(guest.applied.size() >= ticks);

  int power_hits = 0;
  for (std::size_t tick = 0; tick < ticks; ++tick) {
    CAPTURE(tick);
    const TickInput& h = host.applied[tick];
    const TickInput& g = guest.applied[tick];
    REQUIRE(h.left == g.left);
    REQUIRE(h.right == g.right);
    REQUIRE(h.pause_toggle == g.pause_toggle);
    REQUIRE(host.states[tick] == guest.states[tick]);
    power_hits += h.left.power_hit + h.right.power_hit;
  }
  // The inputs really came from both machines
  CHECK(power_hits > 500);
}
