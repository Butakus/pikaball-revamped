#ifndef PIKA_WINDOW_HPP
#define PIKA_WINDOW_HPP

#include <memory>
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "pika_sound.hpp"

namespace pika {

/**
 * A class to handle SDL resources (Window, Renderer, Audio, etc.)
 * Owns and manages the SDL objects.
 * It is responsible for calling SDL_Init and SDL_Quit.
 */
class SDLSystem {
public:
  using SDL_Window_ptr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
  using SDL_Renderer_ptr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
  using SDL_Texture_ptr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
  using TTF_Font_ptr = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>;

  SDLSystem();
  ~SDLSystem();
  SDLSystem(SDLSystem const&) = delete;
  SDLSystem(SDLSystem &&) = delete;
  SDLSystem &operator=(SDLSystem const&) = delete;
  SDLSystem &operator=(SDLSystem &&) = delete;

  /** Get a non-owning pointer to the renderer
   * @return a non-owning pointer to the SDL renderer for this window
   */
  [[nodiscard]] SDL_Renderer* get_renderer() const { return renderer_.get(); }

  /** Get a non-owning pointer to the sound system
   * @return a non-owning pointer to the sound system to play sounds
   */
  [[nodiscard]] PikaSound* get_sound() const { return sound_.get(); }

  /** Get a non-owning pointer to the TTF font for rendering text
   * @return a non-owning pointer to the TTF text font
   */
  [[nodiscard]] TTF_Font* get_font() const { return text_font_.get(); }

  /** Get a non-owning pointer to the sprite sheet texture
   * @return a non-owning pointer to the SDL texture with the sprite sheet
   */
  [[nodiscard]] SDL_Texture* get_sprite_sheet() const { return sprite_sheet_.get(); }

private:
  SDL_Window_ptr window_;
  SDL_Renderer_ptr renderer_;
  std::unique_ptr<PikaSound> sound_;
  TTF_Font_ptr text_font_;

  // Objects
  SDL_Texture_ptr sprite_sheet_ {nullptr, SDL_DestroyTexture};

  /** Load a new sprite sheet from disk and create a texture */
  void load_sprite_sheet();
};

} // namespace pika

#endif //PIKA_WINDOW_HPP
