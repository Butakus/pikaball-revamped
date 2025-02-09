#include "volley_view.hpp"
#include "pikaball/common.hpp"
#include "pikaball/sprites.hpp"

namespace pika::view {

VolleyView::VolleyView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet) :
  View(renderer, sprite_sheet)
{}

GameState VolleyView::update() {
  if (renderer_ == nullptr) {
    return GameState::Round;
  }

  // TODO: Game logic

  // Load the background texture if not already initialized
  if (!background_texture_) {
    preload_background();
  }

  // Render the static background
  SDL_RenderTexture(renderer_, background_texture_.get(), nullptr, nullptr);
  // Waves and clouds
  render_waves();
  render_clouds();

  SDL_RenderPresent(renderer_);
  frame_counter_++;
  return GameState::Round;
}

void VolleyView::start() {
  frame_counter_ = 0;
  input_1_ = {};
  input_2_ = {};
  if (!background_texture_) {
    preload_background();
  }

  // TODO: Other initialization / reset
}


void VolleyView::set_input(const PlayerInput &input_1,
                           const PlayerInput &input_2) {
  input_1_ = input_1;
  input_2_ = input_2;
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


} // pika::view namespace
