#ifndef PIKA_CLOUD_HPP
#define PIKA_CLOUD_HPP
#include <array>

#include <SDL3/SDL_rect.h>

namespace pika {
class Cloud {
public:
  Cloud();

  /** Updates the cloud coordinates (and size). Must be called on every frame. */
  void update();

  /**
   * Get the cloud's current position and size.
   * @return An SDL_Rect with the current position and size.
   */
  [[nodiscard]] SDL_Rect get_rect() const;

private:
  // Actual pixel coordinates of the top-left corner
  int x_;
  int y_;
  // Cloud velocity in the x-axis
  int velocity_x_;
  // Sequence from [0-11] to resize the sprite
  int resize_factor_seq_;
};

class CloudSet {
  /// The number of clouds to draw on the screen
  constexpr static unsigned int num_clouds {10};
public:
  CloudSet() = default;
  void update();
  [[nodiscard]] std::array<SDL_Rect, num_clouds> get_clouds() const;

private:
  std::array<Cloud, num_clouds> clouds_;
};

} // namespace pika

#endif // PIKA_CLOUD_HPP
