#ifndef PIKA_PLAYER_VIEW_HPP
#define PIKA_PLAYER_VIEW_HPP

#include <map>

#include <SDL3/SDL_render.h>
#include <pikaball/physics/player.hpp>
#include <pikaball/sprites.hpp>

namespace pika::view {

class PlayerView {
public:
  explicit PlayerView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet);

  void draw_player(const Player& player) const;
private:
  SDL_Renderer* renderer_ {nullptr};
  SDL_Texture* sprite_sheet_ {nullptr};
};

static const std::map<PlayerState, std::array<SDL_FRect, 5>> player_animations = {
  {PlayerState::Normal, sprite::pikachu_normal_animation},
  {PlayerState::Jumping, sprite::pikachu_jump_animation},
  {PlayerState::PowerHit, sprite::pikachu_hit_animation},
  {PlayerState::Diving, sprite::pikachu_dive_animation},
  {PlayerState::AfterDiving, sprite::pikachu_after_diving_animation},  // Actually only one
  {PlayerState::Winner, sprite::pikachu_winner_animation},
  {PlayerState::Loser, sprite::pikachu_loser_animation}
};

} // namespace pika::view

#endif // PIKA_PLAYER_VIEW_HPP
