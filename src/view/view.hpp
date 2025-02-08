#ifndef PIKA_VIEW_HPP
#define PIKA_VIEW_HPP

#include <memory>
#include <SDL3/SDL_render.h>
#include "pikaball/game_state.hpp"

namespace pika::view {

class View {
public:
  explicit View(SDL_Renderer* renderer, SDL_Texture* sprite_sheet) :
    renderer_(renderer),
    sprite_sheet_(sprite_sheet)
  {}
  virtual ~View() = default;

  View(View const&) = delete;
  View(View &&) = delete;
  View &operator=(View const&) = delete;
  View &operator=(View &&) = delete;

  /**
   * Update the scene state and render the next frame.
   * @return The next game state
   */
  virtual GameState update() = 0;

  /**
   * Start (or restart) the view, resetting the state and the frame counter.
   */
  virtual void start() = 0;

protected:
  // Convenient type alias to hold owning textures
  using SDL_Texture_ptr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
  // Non-owning pointer to the window renderer. Should be set in the constructor
  SDL_Renderer* renderer_ {nullptr};
  // Non-owning pointer to the sprite sheet texture. Should be set in the constructor
  SDL_Texture* sprite_sheet_ {nullptr};

  // Frame counter to control inner state and transitions. Should be incremented after each update
  unsigned int frame_counter_ {0};
};

} // namespace pika::view

#endif // PIKA_VIEW_HPP
