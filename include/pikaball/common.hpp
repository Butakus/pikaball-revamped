#ifndef PIKA_COMMON_HPP
#define PIKA_COMMON_HPP

namespace pika {

// Screen dimensions
// TODO: Take this from config file?
static constexpr int screen_width = 432;
static constexpr int screen_height = 304;
static constexpr int screen_h_width = screen_width / 2;
static constexpr int screen_h_height = screen_height / 2;
static constexpr char window_name [] = "PikaBall MLP Revamped";

static constexpr unsigned long ns_per_second = 1000000000;


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


} // namespace pika::txt

} // namespace pika

#endif // PIKA_COMMON_HPP
