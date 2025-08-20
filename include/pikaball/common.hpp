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

static constexpr unsigned long ns_per_second = 1000000000;

// Game assets
static constexpr char sprite_sheet_filename [] = "assets/images/sprite_sheet_new.bmp";
static constexpr char music_background_filename [] = "assets/sounds/bgm.mp3";
static constexpr char sound_pi_filename [] = "assets/sounds/pi.wav";
static constexpr char sound_pika_filename [] = "assets/sounds/pika.wav";
static constexpr char sound_chu_filename [] = "assets/sounds/chu.wav";
static constexpr char sound_pikachu_filename [] = "assets/sounds/pikachu.wav";
static constexpr char sound_pipikachu_filename [] = "assets/sounds/pipikachu.wav";
static constexpr char sound_ball_hit_filename [] = "assets/sounds/ball_hit.wav";
static constexpr char sound_ball_ground_filename [] = "assets/sounds/ball_ground.wav";
static constexpr char text_font_filename [] = "assets/font.ttf";

static constexpr unsigned int text_font_size = 40;

} // namespace pika

#endif // PIKA_COMMON_HPP
