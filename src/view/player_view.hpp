#ifndef PIKA_PLAYER_VIEW_HPP
#define PIKA_PLAYER_VIEW_HPP

#include <algorithm>
#include <array>
#include <cstddef>

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_render.h>
#include <pikaball/physics/player.hpp>
#include <pikaball/sprites.hpp>

namespace pika::view {

/** Get the animation frames for a player state */
[[nodiscard]] inline const std::array<SDL_FRect, 5>& player_animation(const PlayerState state) {
  switch (state) {
    case PlayerState::Normal:      return sprite::pikachu_normal_animation;
    case PlayerState::Jumping:     return sprite::pikachu_jump_animation;
    case PlayerState::PowerHit:    return sprite::pikachu_hit_animation;
    case PlayerState::Diving:      return sprite::pikachu_dive_animation;
    case PlayerState::AfterDiving: return sprite::pikachu_after_diving_animation;
    case PlayerState::Winner:      return sprite::pikachu_winner_animation;
    case PlayerState::Loser:       return sprite::pikachu_loser_animation;
  }
  // Only reachable with an invalid enum value: degrade instead of undefined behaviour
  SDL_assert(false && "Unknown PlayerState in PlayerView");
  return sprite::pikachu_normal_animation;
}

class PlayerView {
public:
  explicit PlayerView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet) :
    renderer_(renderer),
    sprite_sheet_(sprite_sheet)
  {}

  /**
   * Draw the player
   * @param player The Player object from the game Physics
   */
  void draw_player(const Player& player) const {
    const PlayerState state = player.state();
    const auto& anim_frames = player_animation(state);
    // Debug: stop at the bug (SDL_assert). Release: clamp instead of reading out of bounds.
    const auto frame = static_cast<std::size_t>(player.anim_frame_number());
    SDL_assert(frame < anim_frames.size());
    const SDL_FRect& src_sprite = anim_frames[std::min(frame, anim_frames.size() - 1)];
    const int player_width = static_cast<int>(src_sprite.w);
    const int player_height = static_cast<int>(src_sprite.h);

    // Initially, only flip the sprite of the right side
    SDL_FlipMode flip = player.side() == FieldSide::Left ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    // If the player is diving... Only flip it when diving to the left
    if (state == PlayerState::Diving || state == PlayerState::AfterDiving) {
      if (player.side() == FieldSide::Left && player.diving_direction() == DirX::Left) {
        flip = SDL_FLIP_HORIZONTAL;
      }
      else if (player.side() == FieldSide::Right && player.diving_direction() == DirX::Right) {
        flip = SDL_FLIP_NONE;
      }
    }

    const int x = player.x() - player_width / 2;
    const int y = player.y() - player_height / 2;
    const SDL_FRect player_dst {
      .x = static_cast<float>(x),
      .y = static_cast<float>(y),
      .w = src_sprite.w,
      .h = src_sprite.h,
    };
    SDL_RenderTextureRotated(renderer_, sprite_sheet_,
                             &src_sprite, &player_dst,
                             0.0, nullptr, flip);
  }

  /**
   * Draw the shadow of the player
   * @param player The Player object from the game Physics
   */
  void draw_shadow(const Player& player) const {
    const SDL_FRect shadow_dst {
      .x = static_cast<float>(player.x()) - sprite::objects_shadow.w / 2,
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

#endif // PIKA_PLAYER_VIEW_HPP
