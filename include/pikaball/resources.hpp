#ifndef PIKA_RESOURCES_HPP
#define PIKA_RESOURCES_HPP
#include <map>
#include "battery/embed.hpp"

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

static const std::map<std::string, b::EmbedInternal::EmbeddedFile> resource_map {
  { "assets/images/sprite_sheet_new.png", b::embed<"../assets/images/sprite_sheet_new.png">()},
  { "assets/sounds/bgm.mp3", b::embed<"../assets/sounds/bgm.mp3">()},
  { "assets/sounds/pi.wav", b::embed<"../assets/sounds/pi.wav">()},
  { "assets/sounds/pika.wav", b::embed<"../assets/sounds/pika.wav">()},
  { "assets/sounds/chu.wav", b::embed<"../assets/sounds/chu.wav">()},
  { "assets/sounds/pikachu.wav", b::embed<"../assets/sounds/pikachu.wav">()},
  { "assets/sounds/pipikachu.wav", b::embed<"../assets/sounds/pipikachu.wav">()},
  { "assets/sounds/ball_hit.wav", b::embed<"../assets/sounds/ball_hit.wav">()},
  { "assets/sounds/ball_ground.wav", b::embed<"../assets/sounds/ball_ground.wav">()},
  { "assets/font.ttf", b::embed<"../assets/font.ttf">()},
};

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
  if (embed::resource_map.contains(filename)) {
    const auto embed_data = embed::resource_map.at(filename);
    SDL_Log("Loading embedded %s | %zu bytes", filename, embed_data.size());
    resource_data = SDL_IOFromConstMem(embed_data.data(), embed_data.size());
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
