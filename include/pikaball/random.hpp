
#ifndef PIKA_RANDOM_HPP
#define PIKA_RANDOM_HPP

#include <cstdint>
#include <random>

namespace pika {

/**
 * The random number engine shared by the whole game.
 * It is seeded from std::random_device on first use.
 */
inline std::mt19937& rng() {
  static std::mt19937 engine {std::random_device{}()};
  return engine;
}

/**
 * Re-seed the random number engine, to get a repeatable sequence of numbers.
 * Used by the tests. The game itself does not need it.
 * @param seed The new seed
 */
inline void seed_rng(const std::uint32_t seed) {
  rng().seed(seed);
}

/**
 * Return a random integer in the range [0, 32767]
 *
 * The machine code of the original game use "_rand()" function in Visual Studio 1988 Library.
 * This version generates the random numbers from a uniform distribution using the STL.
 * Actual implementation is delegated to the compiler.
 */
inline std::uint16_t rand_int() {
  std::uniform_int_distribution<std::uint16_t> dist(0, 32767);
  return dist(rng());
}

} // namespace pika

#endif // PIKA_RANDOM_HPP
