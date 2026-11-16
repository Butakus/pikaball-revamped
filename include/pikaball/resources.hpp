#ifndef PIKA_RESOURCES_HPP
#define PIKA_RESOURCES_HPP

#include <filesystem>
#include <cstddef>
#include <string>
#include "SDL3/SDL.h"
#include "pikaball/sdl_error.hpp"

#ifdef PIKA_EMBED_ASSETS
#include <array>
#include "battery/pika_embed.hpp"
#endif

namespace pika {

// Game assets: Filenames are also used as resource keys
static constexpr char sprite_sheet_filename [] = "assets/images/sprite_sheet.png";
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

#ifdef PIKA_EMBED_ASSETS
namespace embed {

static const std::array<pika::b::EmbedInternal::EmbeddedFile, 10> resource_list {
  {
    { pika::b::embed<"assets/images/sprite_sheet.png">() },
    { pika::b::embed<"assets/sounds/bgm.mp3">() },
    { pika::b::embed<"assets/sounds/pi.wav">() },
    { pika::b::embed<"assets/sounds/pika.wav">() },
    { pika::b::embed<"assets/sounds/chu.wav">() },
    { pika::b::embed<"assets/sounds/pikachu.wav">() },
    { pika::b::embed<"assets/sounds/pipikachu.wav">() },
    { pika::b::embed<"assets/sounds/ball_hit.wav">() },
    { pika::b::embed<"assets/sounds/ball_ground.wav">() },
    { pika::b::embed<"assets/font.ttf">() },
  }
};

} // namespace pika::embed
#endif

inline std::string build_resource_path(const char* filename) {
#ifdef PIKA_ASSETS_RUNTIME_DIR
  std::string runtime_dir = PIKA_ASSETS_RUNTIME_DIR;
  std::string relative_path = filename;
  static constexpr const char* prefix = "assets/";
  static constexpr std::size_t prefix_size = 7;
  if (relative_path.compare(0, prefix_size, prefix) == 0) {
    relative_path.erase(0, prefix_size);
  }
  std::filesystem::path full_path = std::filesystem::path(runtime_dir) / relative_path;
  return full_path.lexically_normal().string();
#else
  const char* base_path = SDL_GetBasePath();
  if (base_path != nullptr) {
    const std::filesystem::path full_path = std::filesystem::path(base_path) / filename;
    return full_path.lexically_normal().string();
  }
  return filename;
#endif
}

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

#ifdef PIKA_EMBED_ASSETS
  for (const auto & res : embed::resource_list) {
    if (res.filename() == filename) {
      SDL_Log("Loading embedded %s | %zu bytes", filename, res.size());
      resource_data = SDL_IOFromConstMem(res.data(), res.size());
      break;
    }
  }
#endif

  if (resource_data == nullptr) {
    const std::string resource_path = build_resource_path(filename);
    SDL_Log("Loading resource from file: %s", resource_path.c_str());
    resource_data = SDL_IOFromFile(resource_path.c_str(), "r");
    if (resource_data == nullptr) {
      throw sdl_error(std::string("Failed to load resource '") + filename + "'");
    }
  }

  return resource_data;
}

} // namespace pika

#endif // PIKA_RESOURCES_HPP
