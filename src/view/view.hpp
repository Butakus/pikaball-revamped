#ifndef PIKA_VIEW_HPP
#define PIKA_VIEW_HPP

#include <memory>
#include <SDL3/SDL_render.h>

namespace pika::view {

class View {
public:
  explicit View(SDL_Texture* sprite_sheet) : sprite_sheet_(sprite_sheet) {}
  virtual ~View() = default;

  View(View const&) = delete;
  View(View &&) = delete;
  View &operator=(View const&) = delete;
  View &operator=(View &&) = delete;

  virtual void render(SDL_Renderer* renderer) = 0;
protected:
  // Convenient type alias to hold owning textures
  using SDL_Texture_ptr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
  // Non-owning pointer to the sprite sheet texture. Should be set in the constructor
  SDL_Texture* sprite_sheet_ {nullptr};
};

} // namespace pika::view

#endif // PIKA_VIEW_HPP
