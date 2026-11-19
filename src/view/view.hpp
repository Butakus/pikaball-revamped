#ifndef PIKA_VIEW_HPP
#define PIKA_VIEW_HPP

#include <algorithm>
#include <memory>
#include <SDL3/SDL_render.h>
#include <pikaball/common.hpp>

namespace pika::view {

class View {
public:
  // Convenient type alias to hold owning textures
  using SDL_Texture_ptr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;

  virtual ~View() = default;

  View(View const&) = delete;
  View(View &&) = delete;
  View &operator=(View const&) = delete;
  View &operator=(View &&) = delete;

  /**
   * TODO
   * @param renderer
   * @param sprite_sheet
   */
  explicit View(SDL_Renderer* renderer, SDL_Texture* sprite_sheet) :
    renderer_(renderer),
    sprite_sheet_(sprite_sheet)
  {}

  /**
   * Start (or restart) the view, resetting the state and the frame counter.
   */
  virtual void start() {
    black_fade_alpha_ = 1.0;
  }

  /** Render the black cover over the whole screen with the current fade alpha */
  void render_fade_in_out() const {
    render_black_rect(black_fade_alpha_);
  }

  /**
   * Reduce the current alpha value for the black cover and render it.
   * The black_fade_alpha_ attribute will be updated (within the limits
   * [0.0, 1.0]
   * @param alpha_decrement the alpha decrement for the black cover
   */
  void fade_in(const float alpha_decrement) {
    black_fade_alpha_ = std::max(0.0f, black_fade_alpha_ - alpha_decrement);
    render_fade_in_out();
  }

  /**
   * Increase the current alpha value for the black cover and render it.
   * The black_fade_alpha_ attribute will be updated (within the limits [0.0, 1.0]
   * @param alpha_increment the alpha increment for the black cover
   */
  void fade_out(const float alpha_increment) {
    black_fade_alpha_ = std::min(1.0f, black_fade_alpha_ + alpha_increment);
    render_fade_in_out();
  }

  /**
   * Get the current alpha value of the black cover texture
   * @return the current alpha value
   */
  [[nodiscard]] float get_fade_alpha() const { return black_fade_alpha_; }

protected:
  // Non-owning pointer to the window renderer. Should be set in the constructor
  SDL_Renderer* renderer_ {nullptr};
  // Non-owning pointer to the sprite sheet texture. Should be set in the constructor
  SDL_Texture* sprite_sheet_ {nullptr};

  // Alpha of the black cover used for fade-in/fade-out effects.
  // Default initialized to one because the fade-in effect usually goes first
  float black_fade_alpha_ {1.0f};

  /**
   * Draw a semi-transparent black rectangle (fades and menu backgrounds).
   * @param alpha Opacity in [0.0, 1.0]. Nothing is drawn if it is zero.
   * @param rect The area to cover, or nullptr for the whole screen.
   */
  void render_black_rect(const float alpha, const SDL_FRect* rect = nullptr) const {
    if (alpha <= 0.0f) {
      return;
    }
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColorFloat(renderer_, 0.0f, 0.0f, 0.0f, alpha);
    SDL_RenderFillRect(renderer_, rect);
  }
};

} // namespace pika::view

#endif // PIKA_VIEW_HPP
