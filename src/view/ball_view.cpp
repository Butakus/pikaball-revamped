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

  // Get the current animation frame
  SDL_FRect src_rect = sprite::ball_hyper;
  if (ball.rotation() < 5) {
    src_rect = sprite::ball_animation[ball.rotation()];
  }

  // Draw Ball
  SDL_FRect f_dst;
  const SDL_Rect ball_dst {
    .x = x,
    .y = y,
    .w = ball_width,
    .h = ball_height,
  };

  SDL_RectToFRect(&ball_dst, &f_dst);
  SDL_RenderTexture(renderer_, sprite_sheet_, &src_rect, &f_dst);

  // For punch effect, refer to FUN_00402ee0
  if (ball.punch_effect_radius() > 0) {
    const int punch_h_size = static_cast<int>(ball.punch_effect_radius());
    const int px = static_cast<int>(ball.punch_effect_x()) - punch_h_size;
    const int py = static_cast<int>(ball.punch_effect_y()) - punch_h_size;
    const SDL_Rect punch_dst {
      .x = px,
      .y = py,
      .w = 2 * punch_h_size,
      .h = 2 * punch_h_size
    };
    SDL_RectToFRect(&punch_dst, &f_dst);
    SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::ball_punch, &f_dst);
  }
  if (ball.power_hit()) {
    // The bal was hit hard. Draw a trailing effect
    const int ball_hyper_x = ball.trailing_x()[0];
    const int ball_hyper_y = ball.trailing_y()[0];
    const int ball_trail_x = ball.trailing_x()[1];
    const int ball_trail_y = ball.trailing_y()[1];
    const SDL_Rect hyper_dst {
      .x = ball_hyper_x,
      .y = ball_hyper_y,
      .w = ball_width,
      .h = ball_height,
    };
    const SDL_Rect trail_dst {
      .x = ball_trail_x,
      .y = ball_trail_y,
      .w = ball_width,
      .h = ball_height,
    };
    // Draw trailing effect (ball_hyper and ball_trail)
    SDL_RectToFRect(&hyper_dst, &f_dst);
    SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::ball_hyper, &f_dst);
    SDL_RectToFRect(&trail_dst, &f_dst);
    SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::ball_trail, &f_dst);
  }
}


} // namespace pika::view
