#ifndef PIKA_WAVE_HPP
#define PIKA_WAVE_HPP

#include <pikaball/random.hpp>
#include <array>

namespace pika::view {
/**
 * Class representing the wave physics.
 * Does not handle textures, but only the time and the position on the y axis of the wave.
 */
class Wave {
  /// The number of wave tiles in the screen (one every 16 pixels)
  constexpr static unsigned int num_waves {432 / 16};
public:
  /** Updates the wave coordinates. Must be called on every frame. */
  void update() {
    vertical_position_ += vertical_velocity_;
    if (vertical_position_ > 32) {
      vertical_position_ = 32;
      vertical_velocity_ = -1;
    }
    else if (vertical_position_ < 0 && vertical_velocity_ < 0) {
      vertical_velocity_ = 2;
      vertical_position_ = -(rand_int() % 40);
    }

    for (auto& wave_position : wave_coords_) {
      wave_position = 314 - vertical_position_ + (rand_int() % 3);
    }
  }

  /**
   * Get the wave coordinates.
   * @return a const reference to the array of coordinates.
   */
  [[nodiscard]] const std::array<int, num_waves>& get_coords() const { return wave_coords_; }
private:
  std::array<int, num_waves> wave_coords_ {314};
  int vertical_position_ {0};
  int vertical_velocity_ {2};
};

} // namespace pika::view

#endif // PIKA_WAVE_HPP
