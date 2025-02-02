#ifndef PIKA_WAVE_HPP
#define PIKA_WAVE_HPP
#include <array>

// The number of wave tiles in the screen (one every 16 pixels)
constexpr size_t NUM_WAVES = 432 / 16;

/**
 * Class representing the wave physics.
 * Does not handle textures, but only the time and the position on the y axis of the wave.
 */
class Wave {
public:
  /**
   * Updates the wave coordinates. Must be called on every frame.
   */
  void update();

  /**
   * Get the wave coordinates.
   * @return a const reference to the array of coordinates.
   */
  [[nodiscard]] const std::array<int, NUM_WAVES>& get_coords() const;
private:
  std::array<int, NUM_WAVES> wave_coords_ {314};
  int vertical_position_ {0};
  int vertical_velocity_ {2};
};



#endif // PIKA_WAVE_HPP
