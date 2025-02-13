#ifndef PIKA_PLAYER_VIEW_HPP
#define PIKA_PLAYER_VIEW_HPP

#include <SDL3/SDL_render.h>
#include <pikaball/physics/player.hpp>

namespace pika::view {

class PlayerView {
public:
  explicit PlayerView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet);

  void draw_player(const Player& player) const;
private:
  SDL_Renderer* renderer_ {nullptr};
  SDL_Texture* sprite_sheet_ {nullptr};
};

} // namespace pika::view

#endif // PIKA_PLAYER_VIEW_HPP
