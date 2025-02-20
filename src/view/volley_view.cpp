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

  switch (volley_game_state_) {
    case VolleyGameState::NewGame:
      render_game_start();
      // TODO: Maybe check transitions after rendering and updating
      if (frame_counter_ >= new_game_frames) {
        volley_game_state_ = VolleyGameState::StartRound;
      }
    break;
    case VolleyGameState::StartRound:
    break;
    case VolleyGameState::PlayRound:
    break;
    case VolleyGameState::EndRound:
    break;
    case VolleyGameState::GameEnd:
    break;
  }

  SDL_RenderPresent(renderer_);
  frame_counter_++;

  if (volley_game_state_ != VolleyGameState::NewGame) {
    const bool ball_touching_ground = physics_->update(input_left_, input_right_);
    // TODO
    (void) ball_touching_ground;
  }
  return GameState::VolleyGame;
}

void VolleyView::start() {
  frame_counter_ = 0;
  input_left_ = {};
  input_right_ = {};
  if (!background_texture_) {
    preload_background();
  }

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
  constexpr int w = static_cast<int>(sprite::msg_game_start.w);
  constexpr int h = static_cast<int>(sprite::msg_game_start.h);
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
