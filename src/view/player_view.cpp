#include "player_view.hpp"

namespace pika::view {

PlayerView::PlayerView(SDL_Renderer *renderer, SDL_Texture *sprite_sheet) :
  renderer_(renderer),
  sprite_sheet_(sprite_sheet)
{}

void PlayerView::draw_player(const Player &player) const {
  const PlayerState state = player.state();
  const SDL_FRect src_sprite = player_animations.at(state)[player.anim_frame_number()];
  const int player_width = static_cast<int>(src_sprite.w);
  const int player_height = static_cast<int>(src_sprite.h);

  // Initially, only flip the sprite of the right side
  SDL_FlipMode flip = player.side() == FieldSide::Left ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
  // If the player is diving... Only flip it when diving to the left
  if (state == PlayerState::Diving || state == PlayerState::AfterDiving) {
    if (player.side() == FieldSide::Left && player.diving_direction() == -1) {
      flip = SDL_FLIP_HORIZONTAL;
    }
    else if (player.side() == FieldSide::Right && player.diving_direction() == 1) {
      flip = SDL_FLIP_NONE;
    }
  }

  const int x = static_cast<int>(player.x() - player_width / 2);
  const int y = static_cast<int>(player.y() - player_height / 2);
  const SDL_FRect player_dst {
    .x = static_cast<float>(x),
    .y = static_cast<float>(y),
    .w = src_sprite.w,
    .h = src_sprite.h,
  };
  // SDL_RenderTexture(renderer_, sprite_sheet_, &src_sprite, &player_dst);
  SDL_RenderTextureRotated(renderer_, sprite_sheet_,
                           &src_sprite, &player_dst,
                           0.0, nullptr, flip);

}

void PlayerView::draw_shadow(const Player &player) const {
  const SDL_FRect shadow_dst {
    .x = static_cast<float>(player.x()) - sprite::objects_shadow.w / 2,
    .y = 273 - sprite::objects_shadow.h / 2,
    .w = sprite::objects_shadow.w,
    .h = sprite::objects_shadow.h,
  };
  SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::objects_shadow, &shadow_dst);
}

} // namespace pika::view