#ifndef PIKA_INTRO_VIEW_HPP
#define PIKA_INTRO_VIEW_HPP

#include "pikaball/common.hpp"
#include "pikaball/input.hpp"
#include "pikaball/sprites.hpp"
#include "view.hpp"

namespace pika::view {

class IntroView final : public View {

public:
// Total number of frames to render the intro
constexpr static unsigned int max_frames = 165;

private:
// Fixed sprite positions
constexpr static SDL_FRect sachi_dst {
  .x = screen_h_width - sprite::msg_sachisoft.w - 15,
  .y = screen_h_height - sprite::msg_sachisoft.h / 2,
  .w = sprite::msg_sachisoft.w,
  .h = sprite::msg_sachisoft.h,
};
constexpr static SDL_FRect mlp_dst {
  .x = screen_h_width + 15,
  .y = screen_h_height - sprite::msg_init_mark_mlp.h / 2,
  .w = sprite::msg_init_mark_mlp.w,
  .h = sprite::msg_init_mark_mlp.h,
};

public:
  ~IntroView() override = default;
  explicit IntroView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet) :
    View(renderer, sprite_sheet)
  {}

  /** Render the intro messages and the fade in/out effects */
  void render(unsigned int frame_counter) {
    if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
      return;
    }

    // Fill the background black
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer_);

    // Render intro sprites
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::msg_sachisoft, &sachi_dst);
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::msg_init_mark_mlp, &mlp_dst);

    // Apply fade-in and fade-out effects
    if (frame_counter <= 25) {
      fade_in(1.0f / 25);
    }
    else if (frame_counter > 100) {
      fade_out(1.0f / 25);
    }

    SDL_RenderPresent(renderer_);
  }
};

} // namespace pika::view

#endif // PIKA_INTRO_VIEW_HPP
