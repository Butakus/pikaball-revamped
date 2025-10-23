#ifndef PIKA_BALL_VIEW_HPP
#define PIKA_BALL_VIEW_HPP

#include <SDL3/SDL_render.h>
#include <pikaball/physics/ball.hpp>
#include <pikaball/sprites.hpp>

namespace pika::view {

class BallView {
public:
  explicit BallView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet) :
    renderer_(renderer),
    sprite_sheet_(sprite_sheet)
  {}

  /**
   * Draw the ball and the punch effect
   * @param ball The Ball object from the game Physics
   */
  void draw_ball(const Ball& ball) const {
    constexpr int ball_width = static_cast<int>(sprite::ball_hyper.w);
    constexpr int ball_height = static_cast<int>(sprite::ball_hyper.h);
    const int x = ball.x() - ball_width / 2;
    const int y = ball.y() - ball_height / 2;

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
      const int punch_h_size = ball.punch_effect_radius();
      const int px = ball.punch_effect_x() - punch_h_size;
      const int py = ball.punch_effect_y() - punch_h_size;
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
          .x = static_cast<float>(ball.trailing_x()[i]) - sprite::ball_hyper.w / 2,
          .y = static_cast<float>(ball.trailing_y()[i]) - sprite::ball_hyper.h / 2,
          .w = ball_width,
          .h = ball_height,
        };
        const auto ball_sprite = sprite::ball_trail_animation[i];  // Hyper or trail
        SDL_RenderTexture(renderer_, sprite_sheet_, &ball_sprite, &hyper_dst);
      }
    }
  }

  /**
   * Draw the shadow of the ball
   * @param ball The Ball object from the game Physics
   */
  void draw_shadow(const Ball& ball) const {
    const SDL_FRect shadow_dst {
      .x = static_cast<float>(ball.x()) - sprite::objects_shadow.w / 2,
      .y = 273 - sprite::objects_shadow.h / 2,
      .w = sprite::objects_shadow.w,
      .h = sprite::objects_shadow.h,
    };
    SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::objects_shadow, &shadow_dst);
  }

private:
  SDL_Renderer* renderer_ {nullptr};
  SDL_Texture* sprite_sheet_ {nullptr};

};

} // namespace pika::view

#endif // PIKA_BALL_VIEW_HPP
