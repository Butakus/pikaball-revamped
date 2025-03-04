#ifndef PIKA_CLOUD_HPP
#define PIKA_CLOUD_HPP

#include <pikaball/random.hpp>
#include <array>
#include <SDL3/SDL_rect.h>

namespace pika::view {
class Cloud {
public:
  Cloud() :
    x_ {-68 + rand_int() % (432 + 68)},
    y_ {rand_int() % 152},
    velocity_x_ {1 + rand_int() % 2},
    resize_factor_seq_ {rand_int() % 11}
  {}

  /** Updates the cloud coordinates (and size). Must be called on every frame. */
  void update() {
    x_ += velocity_x_;
    if (x_ > 432) {
      // If the cloud left the screen, generate a new one
      x_ = -68;
      y_ = rand_int() % 152;
      velocity_x_ = 1 + rand_int() % 2;
    }
    resize_factor_seq_ = (resize_factor_seq_ + 1) % 11;
  }

  /**
   * Get the cloud's current position and size.
   * @return An SDL_Rect with the current position and size.
   */
  [[nodiscard]] SDL_Rect get_rect() const {
    const int resize_factor = 5 - std::abs(resize_factor_seq_ - 5);
    return {
      .x = x_ - resize_factor,
      .y = y_ - resize_factor,
      .w = 48 + 2 * resize_factor,
      .h = 24 + 2 * resize_factor
    };
  }

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

  void update() {
    for (auto& cloud : clouds_) {
      cloud.update();
    }
  }

  [[nodiscard]] std::array<SDL_Rect, num_clouds> get_clouds() const {
    std::array<SDL_Rect, num_clouds> cloud_rects {};
    for (size_t i = 0; i < num_clouds; i++) {
      cloud_rects[i] = clouds_[i].get_rect();
    }
    return cloud_rects;
  }

private:
  std::array<Cloud, num_clouds> clouds_;
};

} // namespace pika::view

#endif // PIKA_CLOUD_HPP
