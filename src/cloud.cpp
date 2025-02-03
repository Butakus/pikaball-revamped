#include "random.hpp"
#include "cloud.hpp"

namespace pika {
Cloud::Cloud() :
  x_ {-68 + rand_int() % (432 + 68)},
  y_ {rand_int() % 152},
  velocity_x_ {1 + rand_int() % 2},
  resize_factor_seq_ {rand_int() % 11}
{}

SDL_Rect Cloud::get_rect() const {
  const int resize_factor = 5 - std::abs(resize_factor_seq_ - 5);
  return {
    .x = x_ - resize_factor,
    .y = y_ - resize_factor,
    .w = 48 + 2 * resize_factor,
    .h = 24 + 2 * resize_factor
  };
}

void Cloud::update() {
  x_ += velocity_x_;
  if (x_ > 432) {
    // If the cloud left the screen, generate a new one
    x_ = -68;
    y_ = rand_int() % 152;
    velocity_x_ = 1 + rand_int() % 2;
  }
  resize_factor_seq_ = (resize_factor_seq_ + 1) % 11;
}


std::array<SDL_Rect, CloudSet::num_clouds> CloudSet::get_clouds() const {
  std::array<SDL_Rect, num_clouds> cloud_rects {};
  for (size_t i = 0; i < num_clouds; i++) {
    cloud_rects[i] = clouds_[i].get_rect();
  }
  return cloud_rects;
}

void CloudSet::update() {
  for (auto& cloud : clouds_) {
    cloud.update();
  }
}

} // namespace pika

