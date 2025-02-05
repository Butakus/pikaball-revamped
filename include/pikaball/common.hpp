#ifndef PIKA_COMMON_HPP
#define PIKA_COMMON_HPP

namespace pika {

// Screen dimensions
// TODO: Take this from config file?
constexpr unsigned int screen_width = 432;
constexpr unsigned int screen_height = 304;
constexpr unsigned int screen_h_width = screen_width / 2;
constexpr unsigned int screen_h_height = screen_height / 2;
constexpr char window_name [] = "PikaBall MLP Revamped";
constexpr char sprite_sheet_filename [] = "assets/images/sprite_sheet_new.bmp";

} // namespace pika

#endif // PIKA_COMMON_HPP
