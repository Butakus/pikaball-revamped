#ifndef PIKA_VIEW_HPP
#define PIKA_VIEW_HPP

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
  {
    // Create black texture
    black_texture_.reset(SDL_CreateTexture(
      renderer_,
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_TARGET,
      screen_width,
      screen_height
    ));
    // Set the texture scaling mode to nearest interpolation
    SDL_SetTextureScaleMode(black_texture_.get(), SDL_SCALEMODE_NEAREST);
    // Clear the black texture with a black background
    SDL_SetRenderTarget(renderer_, black_texture_.get());
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer_);

    // Set the render target back to the main window
    SDL_SetRenderTarget(renderer_, nullptr);
  }

  /**
   * Start (or restart) the view, resetting the state and the frame counter.
   */
  virtual void start() {
    black_fade_alpha_ = 1.0;
  }

  /** Render the black rectangle over surface with the current alpha */
  void render_fade_in_out() const {
    SDL_SetTextureAlphaModFloat(black_texture_.get(), black_fade_alpha_);
    SDL_RenderTexture(renderer_, black_texture_.get(), nullptr, nullptr);
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

  // Full-black cover texture to do fade-in/fade-out effects
  SDL_Texture_ptr black_texture_ {nullptr, SDL_DestroyTexture};
  // The alpha channel is default initialized to one because tha fade-in effect usually goes first
  float black_fade_alpha_ {1.0f};
};

} // namespace pika::view

#endif // PIKA_VIEW_HPP
