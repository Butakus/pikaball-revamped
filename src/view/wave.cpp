#include "pikaball/random.hpp"
#include "wave.hpp"

namespace pika {
const std::array<int, Wave::num_waves>& Wave::get_coords() const {
   return wave_coords_;
}

void Wave::update() {
   vertical_position_ += vertical_velocity_;
   if (vertical_position_ > 32) {
      vertical_position_ = 32;
      vertical_velocity_ = -1;
   } else if (vertical_position_ < 0 && vertical_velocity_ < 0) {
      vertical_velocity_ = 2;
      vertical_position_ = -(rand_int() % 40);
   }

   for (auto& wave_position : wave_coords_) {
      wave_position = 314 - vertical_position_ + (rand_int() % 3);
   }
}

} // namespace pika
