#ifndef PIKA_SPRITES_HPP
#define PIKA_SPRITES_HPP

#include "SDL3/SDL_rect.h"
#include <array>

/** Pixel locations and sizes of the sprites from the sprite sheet */

namespace pika::sprite {

constexpr SDL_FRect ball_0 {
  .x = 88,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_1 {
  .x = 130,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_2 {
  .x = 172,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_3 {
  .x = 214,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_4 {
  .x = 256,
  .y = 158,
  .w = 40,
  .h = 40
};

// Ball animation
constexpr std::array ball_animation {
  ball_0,
  ball_1,
  ball_2,
  ball_3,
  ball_4,
};

constexpr SDL_FRect ball_hyper {
  .x = 298,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_punch {
  .x = 340,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect ball_trail {
  .x = 382,
  .y = 158,
  .w = 40,
  .h = 40
};

constexpr SDL_FRect msg_game_end {
  .x = 124,
  .y = 64,
  .w = 96,
  .h = 24
};

constexpr SDL_FRect msg_ready {
  .x = 222,
  .y = 64,
  .w = 80,
  .h = 24
};

constexpr SDL_FRect msg_copyright {
  .x = 2,
  .y = 20,
  .w = 360,
  .h = 20
};

// Fight!! message, or "MLP POWAH!!"
constexpr SDL_FRect msg_fight {
  .x = 92,
  .y = 723,
  .w = 160,
  .h = 160
};

constexpr SDL_FRect msg_game_start {
  .x = 304,
  .y = 64,
  .w = 96,
  .h = 24
};

constexpr SDL_FRect msg_sachisoft {
  .x = 386,
  .y = 611,
  .w = 88,
  .h = 110
};

constexpr SDL_FRect msg_pikachu_volleyball {
  .x = 2,
  .y = 530,
  .w = 276,
  .h = 79
};

constexpr SDL_FRect msg_pokemon_tournament {
  .x = 150,
  .y = 90,
  .w = 200,
  .h = 32
};

constexpr SDL_FRect msg_player_1 {
  .x = 2,
  .y = 42,
  .w = 120,
  .h = 20
};

constexpr SDL_FRect msg_player_2 {
  .x = 124,
  .y = 42,
  .w = 120,
  .h = 20
};

constexpr SDL_FRect msg_init_mark_mlp {
  .x = 2,
  .y = 723,
  .w = 88,
  .h = 110
};

constexpr SDL_FRect number_0 {
  .x = 204,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_1 {
  .x = 238,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_2 {
  .x = 272,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_3 {
  .x = 306,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_4 {
  .x = 340,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_5 {
  .x = 374,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_6 {
  .x = 408,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_7 {
  .x = 442,
  .y = 124,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_8 {
  .x = 2,
  .y = 158,
  .w = 32,
  .h = 32
};

constexpr SDL_FRect number_9 {
  .x = 36,
  .y = 158,
  .w = 32,
  .h = 32
};

// Array of number sprites for an easier lookup
constexpr std::array numbers {
  number_0,
  number_1,
  number_2,
  number_3,
  number_4,
  number_5,
  number_6,
  number_7,
  number_8,
  number_9,
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

constexpr SDL_FRect pikachu_normal_0 {
  .x = 2,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_normal_1 {
  .x = 68,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_normal_2 {
  .x = 134,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_normal_3 {
  .x = 200,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_normal_4 {
  .x = 266,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_jump_0 {
  .x = 332,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_jump_1 {
  .x = 398,
  .y = 266,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_jump_2 {
  .x = 2,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_jump_3 {
  .x = 68,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_jump_4 {
  .x = 134,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_hit_0 {
  .x = 200,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_hit_1 {
  .x = 266,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_hit_2 {
  .x = 332,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_hit_3 {
  .x = 398,
  .y = 332,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_hit_4 {
  .x = 2,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_dive_0 {
  .x = 68,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_dive_1 {
  .x = 134,
  .y = 398,
  .w = 64,
  .h = 64
};

// After diving
constexpr SDL_FRect pikachu_after_diving {
  .x = 200,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_winner_0 {
  .x = 266,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_winner_1 {
  .x = 332,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_winner_2 {
  .x = 398,
  .y = 398,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_winner_3 {
  .x = 2,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_winner_4 {
  .x = 68,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_loser_0 {
  .x = 134,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_loser_1 {
  .x = 200,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_loser_2 {
  .x = 266,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_loser_3 {
  .x = 332,
  .y = 464,
  .w = 64,
  .h = 64
};

constexpr SDL_FRect pikachu_loser_4 {
  .x = 398,
  .y = 464,
  .w = 64,
  .h = 64
};


// Pikachu player sprite animations
constexpr std::array pikachu_normal_animation {
  pikachu_normal_0,
  pikachu_normal_1,
  pikachu_normal_2,
  pikachu_normal_3,
  pikachu_normal_4,
};

constexpr std::array pikachu_jump_animation {
  pikachu_jump_0,
  pikachu_jump_1,
  pikachu_jump_2,
  pikachu_jump_3,
  pikachu_jump_4,
};

constexpr std::array pikachu_hit_animation {
  pikachu_hit_0,
  pikachu_hit_1,
  pikachu_hit_2,
  pikachu_hit_3,
  pikachu_hit_4,
};

constexpr std::array pikachu_dive_animation {
  pikachu_dive_0,
  pikachu_dive_1,
  // Extra to make it length 5
  pikachu_dive_0,
  pikachu_dive_1,
  pikachu_dive_0,
};

constexpr std::array pikachu_after_diving_animation {
  pikachu_after_diving,
  // Extra to make it length 5
  pikachu_after_diving,
  pikachu_after_diving,
  pikachu_after_diving,
  pikachu_after_diving,
};

constexpr std::array pikachu_winner_animation {
  pikachu_winner_0,
  pikachu_winner_1,
  pikachu_winner_2,
  pikachu_winner_3,
  pikachu_winner_4,
};

constexpr std::array pikachu_loser_animation {
  pikachu_loser_0,
  pikachu_loser_1,
  pikachu_loser_2,
  pikachu_loser_3,
  pikachu_loser_4,
};

constexpr SDL_FRect sitting_pikachu {
  .x = 280,
  .y = 611,
  .w = 104,
  .h = 104
};

} // end namespace pika::sprite

#endif //PIKA_SPRITES_HPP
