#ifndef PIKA_WINDOW_HPP
#define PIKA_WINDOW_HPP

#include <memory>
#include "SDL3/SDL.h"

/** TODO: Docs
 *
 */
class Window {
public:
  using SDL_Window_ptr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
  using SDL_Renderer_ptr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
  using SDL_Surface_ptr = std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)>;
  using SDL_Texture_ptr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;

  Window();
  ~Window();

  void render() const;
  void set_sprite_sheet(SDL_Texture_ptr new_sprite_sheet);
  void load_sprite_sheet(const std::string& sprite_sheet_filename);

  [[nodiscard]] inline SDL_Renderer* get_renderer() const { return renderer_.get(); }

private:
  SDL_Window_ptr window_;
  SDL_Renderer_ptr renderer_;
  SDL_Surface* window_surface_;
  SDL_Texture_ptr sprite_sheet_ {nullptr, SDL_DestroyTexture};
};

#endif //PIKA_WINDOW_HPP
