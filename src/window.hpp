#ifndef PIKA_WINDOW_HPP
#define PIKA_WINDOW_HPP


#include "SDL3/SDL.h"
#include <memory>

#include "view/view.hpp"

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

  /** Render the next frame */
  void render();

  /**
   * Change the active view that will render the content
   * @param view non-owning pointer to the active view
   */
  void set_view(view::View* view);

  /** Get a non-owning pointer to the renderer */
  [[nodiscard]] inline SDL_Renderer* get_renderer() const { return renderer_.get(); }

  /** Get a non-owning pointer to the sprite sheet texture */
  [[nodiscard]] inline SDL_Texture* get_sprite_sheet() const { return sprite_sheet_.get(); }

private:
  SDL_Window_ptr window_;
  SDL_Renderer_ptr renderer_;
  SDL_Surface* window_surface_;

  // Current game view (intro, menu, volley, etc.)
  view::View* view_ {nullptr};

  // Objects
  SDL_Texture_ptr sprite_sheet_ {nullptr, SDL_DestroyTexture};

  // Frame rate management
  Uint64 last_render_time_ {0};
  Uint64 target_fps_ {25}; // Possible speeds are 20 / 25 / 30 fps
  Uint64 target_time_per_frame_ {1000 / target_fps_};

  /** Load a new sprite sheet from disk and create a texture */
  void load_sprite_sheet();
};

} // namespace pika

#endif //PIKA_WINDOW_HPP
