#include "sdl_system.hpp"
#include "pikaball/common.hpp"

namespace pika {

constexpr uint32_t sdl_init_flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
constexpr uint64_t sdl_window_flags = SDL_WINDOW_RESIZABLE;

SDLSystem::SDLSystem() :
  window_(nullptr, SDL_DestroyWindow),
  renderer_(nullptr, SDL_DestroyRenderer),
  sound_(nullptr),
  text_font_(nullptr, TTF_CloseFont)
{
  if (!SDL_Init(sdl_init_flags)) {
    throw std::runtime_error("Failed to init SDL");
  }
  if (!TTF_Init()) {
    throw std::runtime_error("Failed to init SDL_ttf");
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
    throw std::runtime_error("Failed to create window/renderer");
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
  text_font_.reset(TTF_OpenFont(text_font_filename, text_font_size));
  if(!text_font_)
  {
    SDL_Log( "Could not load TTF font! SDL_ttf error: %s\n", SDL_GetError());
    throw std::runtime_error("Could not load TTF font");
  }

  // Load sprites and build the static background
  SDL_Log("Loading sprites...");
  load_sprite_sheet();
  SDL_Log("Done!");
}

SDLSystem::~SDLSystem() {
  // Force free the window resources before calling SDL_Quit
  if (renderer_) {
      renderer_.reset();
  }
  if (window_) {
      window_.reset();
  }

  if (text_font_) {
    text_font_.reset();
  }

  // Force Sound System to free resources before calling SDL_Quit
  if (sound_) {
      sound_.reset();
  }

  SDL_Quit();
}

void SDLSystem::load_sprite_sheet() {
  // Load BMP from file, and throw if error
  SDL_Surface* sprites_surface = SDL_LoadBMP(sprite_sheet_filename);
  if (sprites_surface == nullptr) {
    SDL_Log( "Unable to load image %s! SDL Error: %s\n", sprite_sheet_filename, SDL_GetError());
    throw std::runtime_error("Failed to load sprite sheet!");
  }
  // Generate the texture and save it
  sprite_sheet_.reset(SDL_CreateTextureFromSurface(renderer_.get(), sprites_surface));
  SDL_SetTextureScaleMode(sprite_sheet_.get(), SDL_SCALEMODE_NEAREST);
  SDL_Log("Sprite sheet size: %dx%d", sprite_sheet_.get()->w, sprite_sheet_.get()->h);
  // Release the temporary surface object
  SDL_DestroySurface(sprites_surface);
}

} // namespace pika