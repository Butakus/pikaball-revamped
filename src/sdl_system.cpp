#include "sdl_system.hpp"
#include "pikaball/common.hpp"

namespace pika {

constexpr uint32_t sdl_init_flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
constexpr uint64_t sdl_window_flags = SDL_WINDOW_RESIZABLE;

SDLSystem::SDLSystem() :
      window_(nullptr, SDL_DestroyWindow),
      renderer_(nullptr, SDL_DestroyRenderer)
{
  if (!SDL_Init(sdl_init_flags)) {
    throw std::runtime_error("Failed to init SDL");
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

  // Load sprites and build the static background
  SDL_Log("Loading sprites...");
  load_sprite_sheet();
  SDL_Log("Done!");
}

SDLSystem::~SDLSystem() {
  // This might be required to free the window resources before calling SDL_Quit
  // if (window_) {
  //     window_.reset();
  // }

  SDL_Quit();
}

void SDLSystem::render() const {
  // Fill the background white
  SDL_SetRenderDrawColor(renderer_.get(), 0xFF, 0xFF, 0xFF, 0xFF );
  SDL_RenderClear(renderer_.get());

  // Update screen
  SDL_RenderPresent(renderer_.get());
  // SDL_Log("Render time: %d ms - %d fps", current_time -  last_render_time_, 1000 / (current_time -  last_render_time_));
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