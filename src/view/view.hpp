#ifndef PIKA_VIEW_HPP
#define PIKA_VIEW_HPP

#include <memory>
#include <SDL3/SDL_render.h>
#include "pikaball/game_state.hpp"

namespace pika::view {

class View {
public:
  explicit View(SDL_Renderer* renderer, SDL_Texture* sprite_sheet);
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

  void render_fade_in_out() const;

  /**
   * Reduce the current alpha value for the black cover and render it.
   * The black_fade_alpha_ attribute will be updated (within the limits
   * [0.0, 1.0]
   * @param alpha_decrement the alpha decrement for the black cover
   */
  void fade_in(float alpha_decrement);
  /**
   * Increase the current alpha value for the black cover and render it.
   * The black_fade_alpha_ attribute will be updated (within the limits [0.0, 1.0]
   * @param alpha_increment the alpha increment for the black cover
   */
  void fade_out(float alpha_increment);

protected:
  // Convenient type alias to hold owning textures
  using SDL_Texture_ptr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
  // Non-owning pointer to the window renderer. Should be set in the constructor
  SDL_Renderer* renderer_ {nullptr};
  // Non-owning pointer to the sprite sheet texture. Should be set in the constructor
  SDL_Texture* sprite_sheet_ {nullptr};

  // Frame counter to control inner state and transitions. Should be incremented after each update
  unsigned int frame_counter_ {0};

  // Full-black cover texture to do fade-in/fade-out effects
  SDL_Texture_ptr black_texture_ {nullptr, SDL_DestroyTexture};
  // The alpha channel is default initialized to one because tha fade-in effect usually goes first
  float black_fade_alpha_ {1.0f};
};

} // namespace pika::view

#endif // PIKA_VIEW_HPP
