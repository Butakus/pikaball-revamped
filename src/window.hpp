#ifndef PIKA_WINDOW_HPP
#define PIKA_WINDOW_HPP

#include "SDL3/SDL.h"
#include <memory>

namespace pika {

/** TODO: Docs
 *
 */
class Window {
public:
  using SDL_Window_ptr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
  using SDL_Renderer_ptr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
  using SDL_Texture_ptr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;

  Window();
  ~Window();

  /** Render the next frame
   * DEPRECATED: It will just render a white background.
   */
  void render() const;

  /** Get a non-owning pointer to the renderer */
  [[nodiscard]] SDL_Renderer* get_renderer() const { return renderer_.get(); }

  /** Get a non-owning pointer to the sprite sheet texture */
  [[nodiscard]] SDL_Texture* get_sprite_sheet() const { return sprite_sheet_.get(); }

private:
  SDL_Window_ptr window_;
  SDL_Renderer_ptr renderer_;
  SDL_Surface* window_surface_;

  // Objects
  SDL_Texture_ptr sprite_sheet_ {nullptr, SDL_DestroyTexture};

  /** Load a new sprite sheet from disk and create a texture */
  void load_sprite_sheet();
};

} // namespace pika

#endif //PIKA_WINDOW_HPP
