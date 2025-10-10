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
static constexpr char sprite_sheet_filename [] = "assets/images/sprite_sheet_new.png";
static constexpr char music_background_filename [] = "assets/sounds/bgm.mp3";
static constexpr char sound_pi_filename [] = "assets/sounds/pi.wav";
static constexpr char sound_pika_filename [] = "assets/sounds/pika.wav";
static constexpr char sound_chu_filename [] = "assets/sounds/chu.wav";
static constexpr char sound_pikachu_filename [] = "assets/sounds/pikachu.wav";
static constexpr char sound_pipikachu_filename [] = "assets/sounds/pipikachu.wav";
static constexpr char sound_ball_hit_filename [] = "assets/sounds/ball_hit.wav";
static constexpr char sound_ball_ground_filename [] = "assets/sounds/ball_ground.wav";
static constexpr char text_font_filename [] = "assets/font.ttf";

static constexpr unsigned int text_font_size = 45;

// Options menu strings
// TODO: Localization?
namespace txt {

static constexpr char str_options [] = "Opciones";
static constexpr char str_opt_music [] = "Musica";
static constexpr char str_opt_speed [] = "Velocidad";
static constexpr char str_opt_points [] = "Puntos";
static constexpr char str_on [] = "ON";
static constexpr char str_off [] = "OFF";
static constexpr char str_slow [] = "Lento";
static constexpr char str_medium [] = "Medio";
static constexpr char str_fast [] = "Rapido";
static constexpr char str_5_pts [] = "5";
static constexpr char str_10_pts [] = "10";
static constexpr char str_15_pts [] = "15";


} // namespace pika::text

} // namespace pika

#endif // PIKA_COMMON_HPP
