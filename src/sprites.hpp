#ifndef PIKA_SPRITES_HPP
#define PIKA_SPRITES_HPP

#include "SDL3/SDL_rect.h"
/** Pixel locations and sizes of the sprites from the sprite sheet */

namespace sprite {
constexpr SDL_FRect ball_ball_0 {
  .x = 88,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_ball_1 {
  .x = 130,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_ball_2 {
  .x = 172,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_ball_3 {
  .x = 214,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_ball_4 {
  .x = 256,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_ball_hyper {
  .x = 298,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_ball_punch {
  .x = 340,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_ball_trail {
  .x = 382,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect messages_common_game_end {
  .x = 124,
  .y = 64,
  .w = 96,
  .h = 24
};

constexpr SDL_FRect messages_common_ready {
  .x = 222,
  .y = 64,
  .w = 80,
  .h = 24
};

constexpr SDL_FRect messages_copyright {
  .x = 2,
  .y = 20,
  .w = 360,
  .h = 20
};

// Fight!! message, or "MLP POWAH!!"
constexpr SDL_FRect messages_fight {
  .x = 92,
  .y = 723,
  .w = 160,
  .h = 160
};

constexpr SDL_FRect messages_ja_game_start {
  .x = 304,
  .y = 64,
  .w = 96,
  .h = 24
};

constexpr SDL_FRect messages_ja_mark {
  .x = 386,
  .y = 611,
  .w = 88,
  .h = 110
};

constexpr SDL_FRect messages_pikachu_volleyball {
  .x = 2,
  .y = 530,
  .w = 276,
  .h = 79
};

constexpr SDL_FRect messages_pokemon_tournament {
  .x = 150,
  .y = 90,
  .w = 200,
  .h = 32
};

constexpr SDL_FRect messages_player_1 {
  .x = 2,
  .y = 42,
  .w = 120,
  .h = 20
};

constexpr SDL_FRect messages_player_2 {
  .x = 124,
  .y = 42,
  .w = 120,
  .h = 20
};

constexpr SDL_FRect messages_init_mark_mlp {
  .x = 2,
  .y = 723,
  .w = 88,
  .h = 110
};

constexpr SDL_FRect number_number_0 {
  .x = 204,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_number_1 {
  .x = 238,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_number_2 {
  .x = 272,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_number_3 {
  .x = 306,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_number_4 {
  .x = 340,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_number_5 {
  .x = 374,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_number_6 {
  .x = 408,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_number_7 {
  .x = 442,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_number_8 {
  .x = 2,
  .y = 158,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_number_9 {
  .x = 36,
  .y = 158,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect objects_black {
  .x = 2,
  .y = 2,
  .w = 8,
  .h = 8
};

constexpr SDL_FRect objects_cloud {
  .x = 100,
  .y = 90,
  .w = 48,
  .h = 24
};

constexpr SDL_FRect objects_ground_line {
  .x = 66,
  .y = 2,
  .w = 16,
  .h = 16
};

constexpr SDL_FRect objects_ground_line_leftmost {
  .x = 84,
  .y = 2,
  .w = 16,
  .h = 16
};

constexpr SDL_FRect objects_ground_line_rightmost {
  .x = 102,
  .y = 2,
  .w = 16,
  .h = 16
};

constexpr SDL_FRect objects_ground_red {
  .x = 120,
  .y = 2,
  .w = 16,
  .h = 16
};

constexpr SDL_FRect objects_ground_yellow {
  .x = 138,
  .y = 2,
  .w = 16,
  .h = 16
};

constexpr SDL_FRect objects_mountain {
  .x = 2,
  .y = 200,
  .w = 432,
  .h = 64
};

constexpr SDL_FRect objects_net_pillar {
  .x = 12,
  .y = 2,
  .w = 8,
  .h = 8
};

constexpr SDL_FRect objects_net_pillar_top {
  .x = 22,
  .y = 2,
  .w = 8,
  .h = 8
};

constexpr SDL_FRect objects_shadow {
  .x = 32,
  .y = 2,
  .w = 32,
  .h = 8
};

constexpr SDL_FRect objects_sky_blue {
  .x = 156,
  .y = 2,
  .w = 16,
  .h = 16
};

constexpr SDL_FRect objects_wave {
  .x = 70,
  .y = 158,
  .w = 16,
  .h = 32
};

constexpr SDL_FRect pikachu_pikachu_0_0 {
  .x = 2,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_0_1 {
  .x = 68,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_0_2 {
  .x = 134,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_0_3 {
  .x = 200,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_0_4 {
  .x = 266,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_1_0 {
  .x = 332,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_1_1 {
  .x = 398,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_1_2 {
  .x = 2,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_1_3 {
  .x = 68,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_1_4 {
  .x = 134,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_2_0 {
  .x = 200,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_2_1 {
  .x = 266,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_2_2 {
  .x = 332,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_2_3 {
  .x = 398,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_2_4 {
  .x = 2,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_3_0 {
  .x = 68,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_3_1 {
  .x = 134,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_4_0 {
  .x = 200,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_5_0 {
  .x = 266,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_5_1 {
  .x = 332,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_5_2 {
  .x = 398,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_5_3 {
  .x = 2,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_5_4 {
  .x = 68,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_6_0 {
  .x = 134,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_6_1 {
  .x = 200,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_6_2 {
  .x = 266,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_6_3 {
  .x = 332,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_pikachu_6_4 {
  .x = 398,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect sitting_pikachu {
  .x = 280,
  .y = 611,
  .w = 104,
  .h = 104
};

} // end namespace sprite

#endif //PIKA_SPRITES_HPP
