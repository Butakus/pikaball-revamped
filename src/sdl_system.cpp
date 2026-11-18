#include "sdl_system.hpp"
#include "pikaball/resources.hpp"
#include "pikaball/sdl_error.hpp"

namespace pika {

constexpr uint32_t sdl_init_flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
constexpr uint64_t sdl_window_flags = SDL_WINDOW_RESIZABLE;

SDLSystem::SDLSystem() {
  if (!SDL_Init(sdl_init_flags)) {
    throw sdl_error("Failed to init SDL");
  }
  // The destructor doesn't run if the constructor throws, so clean up manually
  try {
    init();
  } catch (...) {
    shutdown();
    throw;
  }
}

SDLSystem::~SDLSystem() {
  shutdown();
}

void SDLSystem::init() {
  if (!TTF_Init()) {
    throw sdl_error("Failed to init SDL_ttf");
  }

  SDL_Window* temp_window;
  SDL_Renderer* temp_renderer;
  if (!SDL_CreateWindowAndRenderer(
        window_name,
        screen_width,
        screen_height,
        sdl_window_flags,
        &temp_window,
        &temp_renderer)
  ) {
    throw sdl_error("Failed to create window/renderer");
  }
  window_.reset(temp_window);
  renderer_.reset(temp_renderer);

  // Set a fixed logical size, so rendering can use fixed pixel coordinates
  if (!SDL_SetRenderLogicalPresentation(
    renderer_.get(),
    screen_width,
    screen_height,
    SDL_LOGICAL_PRESENTATION_STRETCH
  )) {
    SDL_Log("Unable to set a fixed logical scale for the renderer! SDL Error: %s\n", SDL_GetError());
    // TODO: Throw?? We actually need this for the physics engine
  }

  // Initialize sound subsystem
  sound_ = std::make_unique<PikaSound>();

  // Load the TTF font
  text_font_.reset(TTF_OpenFontIO(load_resource(text_font_filename), true, text_font_size));
  if(!text_font_)
  {
    throw sdl_error(std::string("Failed to load TTF font '") + text_font_filename + "'");
  }

  // Load sprites and build the static background
  load_sprite_sheet();
}

void SDLSystem::load_sprite_sheet() {
  // Load PNG from embedded data or from files. Throw if error.
  // SDL_Surface* sprites_surface = SDL_LoadPNG_IO(load_resource(sprite_sheet_filename), true);
  SDL_Surface* sprites_surface =
    SDL_LoadPNG_IO(load_resource(sprite_sheet_filename), true);

  if (sprites_surface == nullptr) {
    throw sdl_error(std::string("Failed to load sprite sheet '") + sprite_sheet_filename + "'");
  }
  // Generate the texture and save it
  sprite_sheet_.reset(SDL_CreateTextureFromSurface(renderer_.get(), sprites_surface));
  SDL_SetTextureScaleMode(sprite_sheet_.get(), SDL_SCALEMODE_NEAREST);
  // Release the temporary surface object
  SDL_DestroySurface(sprites_surface);
}

void SDLSystem::shutdown() noexcept {
  // Reverse creation order
  sprite_sheet_.reset();
  text_font_.reset();
  sound_.reset();
  renderer_.reset();
  window_.reset();
  TTF_Quit();
  SDL_Quit();
}

} // namespace pika
