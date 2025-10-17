#ifndef PIKA_RESOURCES_HPP
#define PIKA_RESOURCES_HPP
#include <array>

namespace pika {

// Game assets: Filenames are also used as resource keys
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

namespace embed {
namespace data {

/** Load binary data directly into the code using C23 #embed directive */

static constexpr unsigned char sprite_sheet[] = {
  #embed "../../assets/images/sprite_sheet_new.png"
};
static constexpr unsigned char music_background [] = {
  #embed "../../assets/sounds/bgm.mp3"
};
static constexpr unsigned char sound_pi [] = {
  #embed "../../assets/sounds/pi.wav"
};
static constexpr unsigned char sound_pika [] = {
  #embed "../../assets/sounds/pika.wav"
};
static constexpr unsigned char sound_chu [] = {
  #embed "../../assets/sounds/chu.wav"
};
static constexpr unsigned char sound_pikachu [] = {
  #embed "../../assets/sounds/pikachu.wav"
};
static constexpr unsigned char sound_pipikachu [] = {
  #embed "../../assets/sounds/pipikachu.wav"
};
static constexpr unsigned char sound_ball_hit [] = {
  #embed "../../assets/sounds/ball_hit.wav"
};
static constexpr unsigned char sound_ball_ground [] = {
  #embed "../../assets/sounds/ball_ground.wav"
};
static constexpr unsigned char text_font [] = {
  #embed "../../assets/font.ttf"
};


} // namespace data

/**
 * A utility class to store the data with a filename and its size in bytes
 */
struct Resource {
  const char * filename;
  const unsigned char * data;
  size_t size;
};

static constexpr std::array<Resource, 10> resource_list {{
  { sprite_sheet_filename, data::sprite_sheet, sizeof(data::sprite_sheet) },
  { music_background_filename, data::music_background, sizeof(data::music_background) },
  { sound_pi_filename, data::sound_pi, sizeof(data::sound_pi) },
  { sound_pika_filename, data::sound_pika, sizeof(data::sound_pika) },
  { sound_chu_filename, data::sound_chu, sizeof(data::sound_chu) },
  { sound_pikachu_filename, data::sound_pikachu, sizeof(data::sound_pikachu) },
  { sound_pipikachu_filename, data::sound_pipikachu, sizeof(data::sound_pipikachu) },
  { sound_ball_hit_filename, data::sound_ball_hit, sizeof(data::sound_ball_hit) },
  { sound_ball_ground_filename, data::sound_ball_ground, sizeof(data::sound_ball_ground) },
  { text_font_filename, data::text_font, sizeof(data::text_font) },
}};

} // namespace pika::embed

/**
 * Load a game resource. First try to load it from the binary embedded data.
 * If that fails, load it from disk.
 * If the resource cannot be loaded, an exception is thrown.
 *
 * NOTE: After usage, it is required to manually call SDL_CloseIO to free the memory.
 *   This is usually handled by SDL_LoadPNG(..., bool close_io) and similar functions.
 * @param filename The resource filename (relative to the project root)
 * @return A (owning) pointer to a SDL_IOStream
 */
inline SDL_IOStream* load_resource(const char* filename) {
  SDL_IOStream* resource_data = nullptr;
  for (const auto &[res_filename, data, size] : embed::resource_list) {
    if (strcmp(filename, res_filename) == 0) {
      SDL_Log("Loading embedded %s | %zu bytes", filename, size);
      resource_data = SDL_IOFromConstMem(data, size);
      break;
    }
  }
  if (resource_data == nullptr) {
    SDL_Log("Could not load resource %s from embedded data. Loading from file...", filename);
    resource_data = SDL_IOFromFile(filename, "r");
    if (resource_data == nullptr) {
      SDL_Log( "Unable to load resource %s! SDL Error: %s\n", filename, SDL_GetError());
      throw std::runtime_error("Failed to load resource!");
    }
  }

  return resource_data;
}

} // namespace pika

#endif // PIKA_RESOURCES_HPP
