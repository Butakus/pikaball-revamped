#include "volley_view.hpp"
#include "pikaball/common.hpp"
#include "pikaball/sprites.hpp"

namespace pika::view {

VolleyView::VolleyView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet, Physics* physics) :
  View(renderer, sprite_sheet),
  physics_(physics),
  ball_view_(renderer, sprite_sheet),
  player_view_left_(renderer, sprite_sheet),
  player_view_right_(renderer, sprite_sheet)
{}

GameState VolleyView::update() {
  if (renderer_ == nullptr) {
    return GameState::VolleyGame;
  }

  // Always render the background, clouds and waves
  render_background();
  // Same with the ball and players
  render_physics();
  // And same with the scoreboard
  render_score();

  switch (volley_game_state_) {
    case VolleyGameState::NewGame:
      render_game_start();
      // TODO: Maybe check transitions after rendering and updating
      frame_counter_++;
      if (frame_counter_ >= new_game_frames) {
        volley_game_state_ = VolleyGameState::PlayRound;
      }
    break;
    case VolleyGameState::StartRound:
      // Apply fade in effect
      fade_in(1.0f / 16);
      frame_counter_++;
      // Display blinking "Ready" message
      render_ready_msg();
      if (frame_counter_ >= start_round_frames) {
        // Start the next round
        black_fade_alpha_ = 0.0;
        new_round_ = true;
        volley_game_state_ = VolleyGameState::PlayRound;
      }
    break;
    case VolleyGameState::PlayRound:
      new_round_ = false;
      // Update physics and check if the ball is touching the ground
      if (physics_->update(input_left_, input_right_)) {
        // End of the round
        next_serve_side_ = update_score();
        if (score_left_ >= win_score || score_right_ >= win_score) {
          // Game ended
          physics_->end_game(next_serve_side_);
          frame_counter_ = 0;
          volley_game_state_ = VolleyGameState::GameEnd;
        }
        else {
          // Apply end-of-round effects and start next round
          frame_counter_ = 0;
          black_fade_alpha_ = 0.0f;
          volley_game_state_ = VolleyGameState::EndRound;
        }
      }
    break;
    case VolleyGameState::EndRound:
      // Apply and manage slow motion and fading effects
      // Slow motion will be active for the first 6 frames after the point
      slow_motion_ = frame_counter_ <= 6;
      // After the slow motion, start the fade out effect
      frame_counter_++;
      if (frame_counter_ >= 6) {
        fade_out(1.0f / 16);
      }
      // We keep updating the physics, but without checking the ball
      physics_->update(input_left_, input_right_);
      if (frame_counter_ >= end_round_frames) {
        // Start the next round
        frame_counter_ = 0;
        black_fade_alpha_ = 1.0f;
        physics_->init_round(next_serve_side_);
        volley_game_state_ = VolleyGameState::StartRound;
      }
    break;
    case VolleyGameState::GameEnd:
      frame_counter_++;
      // Draw the "Game end" message
      render_game_end();
      // Check if the user wants to skip the end frames
      if (frame_counter_ > game_end_skip_frames &&
          (input_left_.power_hit || input_right_.power_hit)) {
        return GameState::Intro;
      }
      // End the game after the total animation frames
      if (frame_counter_ > game_end_frames) {
        return GameState::Intro;
      }
      physics_->update(input_left_, input_right_);
    break;
  }

  SDL_RenderPresent(renderer_);

  return GameState::VolleyGame;
}

void VolleyView::start() {
  frame_counter_ = 0;
  black_fade_alpha_ = 1.0f;
  slow_motion_ = false;
  new_round_ = false;
  if (!background_texture_) {
    preload_background();
  }

  input_left_ = {};
  input_right_ = {};
  score_left_ = 0;
  score_right_ = 0;
  next_serve_side_ = FieldSide::Left;
  physics_->restart();
  volley_game_state_ = VolleyGameState::NewGame;
}


void VolleyView::set_input(const PlayerInput &input_left,
                           const PlayerInput &input_right) {
  input_left_ = input_left;
  input_right_ = input_right;
}

void VolleyView::render_background() {
  // Load the background texture if not already initialized
  if (!background_texture_) {
    preload_background();
  }

  // Render the static background
  SDL_RenderTexture(renderer_, background_texture_.get(), nullptr, nullptr);
  // Waves and clouds
  render_waves();
  render_clouds();
}

void VolleyView::render_waves() {
  if (sprite_sheet_ == nullptr) {
    return;
  }
  wave_.update();
  SDL_FRect f_dst;
  SDL_Rect dst = {
    .x = 0,
    .w = 16,
    .h = 32,
  };
  for (const auto& w : wave_.get_coords()) {
    dst.y = w;
    SDL_RectToFRect(&dst, &f_dst);
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::objects_wave, &f_dst);
    dst.x += dst.w;
  }
}

void VolleyView::render_clouds() {
  if (sprite_sheet_ == nullptr) {
    return;
  }
  clouds_.update();
  SDL_FRect f_dst;
  for (const auto& cloud : clouds_.get_clouds()) {
    SDL_RectToFRect(&cloud, &f_dst);
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::objects_cloud, &f_dst);
  }
}

void VolleyView::render_physics() const {
  // First draw the shadows so they don't get on top of the players
  ball_view_.draw_shadow(physics_->ball());
  player_view_left_.draw_shadow(physics_->player(FieldSide::Left));
  player_view_right_.draw_shadow(physics_->player(FieldSide::Right));
  // Render ball and players
  player_view_left_.draw_player(physics_->player(FieldSide::Left));
  player_view_right_.draw_player(physics_->player(FieldSide::Right));
  ball_view_.draw_ball(physics_->ball());
}

void VolleyView::render_game_start() {
  if (sprite_sheet_ == nullptr) {
    return;
  }

  // Apply a fade-in the first 17 frames
  fade_in(1.0f / 17);

  // Estimate the message size and position for the current frame_counter
  static constexpr int w = static_cast<int>(sprite::msg_game_start.w);
  static constexpr int h = static_cast<int>(sprite::msg_game_start.h);
  const int half_width = static_cast<int>(w * frame_counter_ / 50);
  const int half_height = static_cast<int>(h * frame_counter_ / 50);
  const SDL_FRect dst = {
    .x = static_cast<float>(216 - half_width),
    .y = static_cast<float>(50 + 2 * half_height),
    .w = static_cast<float>(2 * half_width),
    .h = static_cast<float>(2 * half_height),
  };
  // Draw the "game start" message
  SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::msg_game_start, &dst);
}

void VolleyView::render_ready_msg() const {
  // Only show the message every 5 frames (5 yes / 5 no)
  if (!sprite_sheet_ || frame_counter_ / 5 % 2 == 0) {
    return;
  }
  constexpr static SDL_FRect dst {
    .x = 176,
    .y = 38,
    .w = sprite::msg_ready.w,
    .h = sprite::msg_ready.h,
  };
  SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::msg_ready, &dst);
}

void VolleyView::render_game_end() const {
  if (sprite_sheet_ == nullptr) {
    return;
  }

  SDL_FRect dst = {
    .x = 216 - sprite::msg_game_end.w / 2,
    .y = 50,
    .w = sprite::msg_game_end.w,
    .h = sprite::msg_game_end.h,
  };
  if (frame_counter_ < 50) {
    // In the first frames, reduce the message size
    // Estimate the message size and position for the current frame_counter
    static constexpr int w = static_cast<int>(sprite::msg_game_end.w);
    static constexpr int h = static_cast<int>(sprite::msg_game_end.h);
    const int width_increment = 2 * static_cast<int>(w * (50 - frame_counter_) / 50);
    const int height_increment = 2 * static_cast<int>(h * (50 - frame_counter_) / 50);
    dst.x -= static_cast<float>(width_increment);
    dst.y -= static_cast<float>(height_increment);
    dst.w += static_cast<float>(2 * width_increment);
    dst.h += static_cast<float>(2 * height_increment);
  }
  // Draw the "game start" message
  SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::msg_game_end, &dst);
}


void VolleyView::render_score() const {
  if (sprite_sheet_ == nullptr) {
    return;
  }

  // Set score sprite positions
  static constexpr SDL_FRect dst_left_tens {
    .x = 14,
    .y = 10,
    .w = sprite::number_0.w,
    .h = sprite::number_0.h,
  };
  static constexpr SDL_FRect dst_left_units {
    .x = 14 + sprite::number_0.w,
    .y = 10,
    .w = sprite::number_0.w,
    .h = sprite::number_0.h,
  };
  static constexpr SDL_FRect dst_right_tens {
    .x = screen_width - 2 * sprite::number_0.w - 14,
    .y = 10,
    .w = sprite::number_0.w,
    .h = sprite::number_0.h,
  };
  static constexpr SDL_FRect dst_right_units {
    .x = screen_width - sprite::number_0.w - 14,
    .y = 10,
    .w = sprite::number_0.w,
    .h = sprite::number_0.h,
  };

  // Draw left score
  const unsigned int units_left = score_left_ % 10;
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::numbers[units_left], &dst_left_units);
  if (score_left_ >= 10) {
    const unsigned int tens_left = score_left_ / 10 % 10;
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::numbers[tens_left], &dst_left_tens);
  }

  // Draw right score
  const unsigned int units_right = score_right_ % 10;
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::numbers[units_right], &dst_right_units);
  if (score_right_ >= 10) {
    const unsigned int tens_right = score_right_ / 10 % 10;
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::numbers[tens_right], &dst_right_tens);
  }

}

FieldSide VolleyView::update_score() {
  if (physics_->ball().punch_effect_x() < ground_h_width) {
    score_right_++;
    return FieldSide::Right;
  }
  score_left_++;
  return FieldSide::Left;
}

void VolleyView::preload_background() {
  if (sprite_sheet_ == nullptr || renderer_ == nullptr) {
    return;
  }
  // Create a target texture to render all the background elements to
  background_texture_.reset(SDL_CreateTexture(
    renderer_,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_TARGET,
    screen_width,
    screen_height
  ));
  // Set the texture scaling mode to nearest interpolation
  SDL_SetTextureScaleMode(background_texture_.get(), SDL_SCALEMODE_NEAREST);

  // Focus the renderer on the target texture
  SDL_SetRenderTarget(renderer_, background_texture_.get());

  // Fill the background white
  SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer_);

  // Build the sky
  SDL_FRect f_dst;
  SDL_Rect dst = {
    // Preset width and height of sky texture
    .w = 16,
    .h = 16,
  };
  for (int i = 0; i < screen_width / 16; i++) {
    for (int j = 0; j < 12; j++) {
      // Set position and render texture
      dst.x = i * 16;
      dst.y = j * 16;
      SDL_RectToFRect(&dst, &f_dst);
      SDL_RenderTexture(
        renderer_, sprite_sheet_, &sprite::objects_sky_blue, &f_dst);
    }
  }
  // Render the mountain sprite
  dst.x = 0;
  dst.y = 188;
  dst.w = 432;
  dst.h = 64;
  SDL_RectToFRect(&dst, &f_dst);
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::objects_mountain, &f_dst);

  // Render the red ground
  dst.y = 248;
  dst.w = 16;
  dst.h = 16;
  for (int i = 0; i < screen_width / 16; i++) {
    dst.x = i * 16;
    SDL_RectToFRect(&dst, &f_dst);
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::objects_ground_red, &f_dst);
  }

  // Render the ground line (the field delimiters)
  dst.x = 0;
  dst.y = 264;
  SDL_RectToFRect(&dst, &f_dst);
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::objects_ground_line_leftmost, &f_dst);
  for (int i = 1; i < screen_width / 16 - 1; i++) {
    dst.x = i * 16;
    SDL_RectToFRect(&dst, &f_dst);
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::objects_ground_line, &f_dst);
  }
  dst.x = screen_width - 16;
  dst.y = 264;
  SDL_RectToFRect(&dst, &f_dst);
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::objects_ground_line_rightmost, &f_dst);

  // Render the yellow ground
  for (int i = 0; i < screen_width / 16; i++) {
    for (int j = 0; j < 2; j++) {
      dst.x = i * 16;
      dst.y = 280 + j * 16;
      SDL_RectToFRect(&dst, &f_dst);
      SDL_RenderTexture(
        renderer_, sprite_sheet_, &sprite::objects_ground_yellow, &f_dst);
    }
  }

  // Render the net (pillar and top)
  dst.x = 213;
  dst.y = 176;
  dst.w = 8;
  dst.h = 8;
  SDL_RectToFRect(&dst, &f_dst);
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::objects_net_pillar_top, &f_dst);
  for (int j = 0; j < 12; j++) {
    dst.y = 184 + j * 8;
    SDL_RectToFRect(&dst, &f_dst);
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::objects_net_pillar, &f_dst);
  }

  // Set the render target back to the main window
  SDL_SetRenderTarget(renderer_, nullptr);
}

} // pika::view namespace
