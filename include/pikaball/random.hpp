
#ifndef PIKA_RANDOM_HPP
#define PIKA_RANDOM_HPP

#include <cstdint>
#include <random>

namespace pika {

/**
 * Seedable random number generator.
 *
 * The machine code of the original game uses the "_rand()" function of the Visual Studio 1988 library.
 * This version uses std::mt19937 and takes the numbers directly from its top 15 bits.
 * A given seed produces the same sequence on every platform.
 */
class Rng {
public:
  explicit Rng(const std::uint32_t seed) : engine_(seed) {}

  /** Restart the sequence with a new seed */
  void seed(const std::uint32_t seed) { engine_.seed(seed); }

  /** Return a random integer in the range [0, 32767] */
  std::uint16_t rand_int() { return static_cast<std::uint16_t>(engine_() >> 17); }

private:
  std::mt19937 engine_;
};

/**
 * Return a random integer in the range [0, 32767] from a generator shared
 * by the whole game, seeded from std::random_device on first use.
 * The physics simulation uses its own Rng.
 */
inline std::uint16_t rand_int() {
  static Rng shared {std::random_device{}()};
  return shared.rand_int();
}

} // namespace pika

#endif // PIKA_RANDOM_HPP
