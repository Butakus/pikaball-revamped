#ifndef PIKA_TEXT_TEXTURE_HPP
#define PIKA_TEXT_TEXTURE_HPP

#include <string>

#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "sdl_error.hpp"
#include "view.hpp"

namespace pika::view {

// Text colours
constexpr SDL_Color text_color_white {255, 255, 255, 255};
// Selected option values in the options menu (red)
constexpr SDL_Color text_color_selected {255, 0, 0, 255};

/**
 * Render a text into a new texture. Does not throw: safe to call every frame.
 * @return The texture, or an empty pointer on failure (details in SDL_GetError())
 */
[[nodiscard]] inline View::SDL_Texture_ptr render_text_texture(
  SDL_Renderer* renderer,
  TTF_Font* text_font,
  const std::string& text,
  const SDL_Color& text_color = text_color_white)
{
  View::SDL_Texture_ptr text_texture {nullptr, SDL_DestroyTexture};
  // Load the text into a temporary surface
  SDL_Surface* text_surface = TTF_RenderText_Solid(text_font, text.c_str(), 0, text_color);
  if (text_surface == nullptr) {
    return text_texture;
  }
  // Generate the texture
  text_texture.reset(SDL_CreateTextureFromSurface(renderer, text_surface));
  // Release the temporary surface object
  SDL_DestroySurface(text_surface);

  return text_texture;
}

/**
 * Render a text into a new texture, for textures created at startup.
 * @throw std::runtime_error if the text cannot be rendered
 */
[[nodiscard]] inline View::SDL_Texture_ptr load_text_texture(
  SDL_Renderer* renderer,
  TTF_Font* text_font,
  const std::string& text,
  const SDL_Color& text_color = text_color_white)
{
  auto text_texture = render_text_texture(renderer, text_font, text, text_color);
  if (!text_texture) {
    throw sdl_error("Failed to render text '" + text + "'");
  }
  return text_texture;
}

} // namespace pika::view

#endif // PIKA_TEXT_TEXTURE_HPP
