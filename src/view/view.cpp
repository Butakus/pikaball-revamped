#include "view.hpp"

#include <pikaball/common.hpp>

namespace pika::view {

View::View(SDL_Renderer* renderer, SDL_Texture* sprite_sheet) :
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

void View::render_fade_in_out() const {
  SDL_SetTextureAlphaModFloat(black_texture_.get(), black_fade_alpha_);
  SDL_RenderTexture(renderer_, black_texture_.get(), nullptr, nullptr);
}

void View::fade_in(const float alpha_decrement) {
  black_fade_alpha_ = std::max(0.0f, black_fade_alpha_ - alpha_decrement);
  render_fade_in_out();
}

void View::fade_out(const float alpha_increment) {
  black_fade_alpha_ = std::min(1.0f, black_fade_alpha_ + alpha_increment);
  render_fade_in_out();
}

}  // namespace pika::view
