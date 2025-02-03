
#ifndef PIKA_RANDOM_HPP
#define PIKA_RANDOM_HPP

#include <random>

namespace pika {
/**
 * Return a random integer in the range [0, 32767]
 *
 * The machine code of the original game use "_rand()" function in Visual Studio 1988 Library.
 * This version generates the random numbers from a uniform distribution using the STL.
 * Actual implementation is delegated to the compiler.
 */
inline uint16_t rand_int() {
   static std::random_device rd;
   static std::mt19937 gen(rd());
   static std::uniform_int_distribution<uint16_t> dist(0, 32767);
   return dist(gen);
}

} // namespace pika

#endif // PIKA_RANDOM_HPP
