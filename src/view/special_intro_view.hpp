#ifndef PIKA_SPECIAL_INTRO_VIEW_HPP
#define PIKA_SPECIAL_INTRO_VIEW_HPP

#include "SDL3/SDL_log.h"
#include "pikaball/common.hpp"
#include "pikaball/sprites.hpp"
#include "view.hpp"

namespace pika::view {

class SpecialIntroView final : public View {

public:
// Total number of frames to render the intro
constexpr static unsigned int max_frames = 300;

private:
// Fixed sprite positions
constexpr static SDL_FRect dino_dst {
  .x = screen_h_width - sprite::msg_special_dino.w / 2,
  .y = screen_h_height - sprite::msg_special_dino.h / 2 - sprite::msg_special_mlp.h / 2,
  .w = sprite::msg_special_dino.w,
  .h = sprite::msg_special_dino.h,
};
constexpr static SDL_FRect mlp_dst {
  .x = screen_h_width - sprite::msg_special_mlp.w / 2,
  .y = screen_h_height - sprite::msg_special_mlp.h / 2 + sprite::msg_special_dino.h / 2,
  .w = sprite::msg_special_mlp.w,
  .h = sprite::msg_special_mlp.h,
};

public:
  ~SpecialIntroView() override = default;
  SpecialIntroView(SpecialIntroView const&) = delete;
  SpecialIntroView(SpecialIntroView &&) = delete;
  SpecialIntroView &operator=(SpecialIntroView const&) = delete;
  SpecialIntroView &operator=(SpecialIntroView &&) = delete;

  explicit SpecialIntroView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet) :
    View(renderer, sprite_sheet)
  {}

  /** Render the intro messages and the fade in/out effects */
  void render(const unsigned int frame_counter) {
    if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
      return;
    }

    // Fill the background black
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer_);

    // Only start rendering dino after 25 frames
    if (frame_counter > 25) {
      SDL_RenderTexture(
        renderer_, sprite_sheet_, &sprite::msg_special_dino, &dino_dst);
    }

    // Apply fade-in and fade-out effects
    if (frame_counter > 25 && frame_counter <= 75) {
      fade_in(1.0f / 50);
    }

    // Render the MLP sprite after fade-in so it appears immediately
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::msg_special_mlp, &mlp_dst);

    if (frame_counter > 120) {
      fade_out(1.0f / 25);
    }
  }
};

} // namespace pika::view

#endif // PIKA_SPECIAL_INTRO_VIEW_HPP
