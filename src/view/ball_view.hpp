#ifndef PIKA_BALL_VIEW_HPP
#define PIKA_BALL_VIEW_HPP

#include <SDL3/SDL_render.h>
#include <pikaball/physics/ball.hpp>

namespace pika::view {

class BallView {
public:
  explicit BallView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet);

  void draw_ball(const Ball& ball) const;
private:
  SDL_Renderer* renderer_ {nullptr};
  SDL_Texture* sprite_sheet_ {nullptr};

};

} // namespace pika::view

#endif // PIKA_BALL_VIEW_HPP
