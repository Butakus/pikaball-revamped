#ifndef PIKA_COMMON_HPP
#define PIKA_COMMON_HPP

namespace pika {

// Screen dimensions
// TODO: Take this from config file?
static constexpr unsigned int screen_width = 432;
static constexpr unsigned int screen_height = 304;
static constexpr unsigned int screen_h_width = screen_width / 2;
static constexpr unsigned int screen_h_height = screen_height / 2;
static constexpr char window_name [] = "PikaBall MLP Revamped";
static constexpr char sprite_sheet_filename [] = "assets/images/sprite_sheet_new.bmp";

static constexpr unsigned long ns_per_second = 1000000000;

} // namespace pika

#endif // PIKA_COMMON_HPP
