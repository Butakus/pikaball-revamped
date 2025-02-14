#include "ball_view.hpp"

#include <pikaball/sprites.hpp>

namespace pika::view {

BallView::BallView(SDL_Renderer *renderer, SDL_Texture *sprite_sheet) :
  renderer_(renderer),
  sprite_sheet_(sprite_sheet)
{}

void BallView::draw_ball(const Ball &ball) const {
  constexpr int ball_width = static_cast<int>(sprite::ball_hyper.w);
  constexpr int ball_height = static_cast<int>(sprite::ball_hyper.h);
  const int x = static_cast<int>(ball.x()) - ball_width / 2;
  const int y = static_cast<int>(ball.y()) - ball_height / 2;

  // First, draw the shadow
  const SDL_FRect shadow_dst {
    .x = static_cast<float>(x),
    .y = 273,
    .w = sprite::objects_shadow.w,
    .h = sprite::objects_shadow.h,
  };
  SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::objects_shadow, &shadow_dst);

  // Get the current animation frame
  SDL_FRect src_rect = sprite::ball_hyper;
  if (ball.rotation() < 5) {
    src_rect = sprite::ball_animation[ball.rotation()];
  }

  // Draw Ball
  const SDL_FRect ball_dst {
    .x = static_cast<float>(x),
    .y = static_cast<float>(y),
    .w = sprite::ball_hyper.w,
    .h = sprite::ball_hyper.h,
  };
  SDL_RenderTexture(renderer_, sprite_sheet_, &src_rect, &ball_dst);

  // For punch effect, refer to FUN_00402ee0
  if (ball.punch_effect_radius() > 0) {
    const int punch_h_size = static_cast<int>(ball.punch_effect_radius());
    const int px = static_cast<int>(ball.punch_effect_x()) - punch_h_size;
    const int py = static_cast<int>(ball.punch_effect_y()) - punch_h_size;
    const SDL_FRect punch_dst {
      .x = static_cast<float>(px),
      .y = static_cast<float>(py),
      .w = static_cast<float>(2 * punch_h_size),
      .h = static_cast<float>(2 * punch_h_size),
    };
    SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::ball_punch, &punch_dst);
  }
  if (ball.power_hit()) {
    // The ball was hit hard. Draw a trailing effect (ball_hyper and ball_trail)
    for (size_t i = 0; i < 2; i++) {
      const SDL_FRect hyper_dst {
        .x = static_cast<float>(ball.trailing_x()[i]),
        .y = static_cast<float>(ball.trailing_y()[i]),
        .w = ball_width,
        .h = ball_height,
      };
      const auto ball_sprite = sprite::ball_trail_animation[i];  // Hyper or trail
      SDL_RenderTexture(renderer_, sprite_sheet_, &ball_sprite, &hyper_dst);
    }
  }
}


} // namespace pika::view
