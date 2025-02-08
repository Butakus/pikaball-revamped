#include "menu_view.hpp"

#include <SDL3/SDL_log.h>
#include <pikaball/common.hpp>
#include <pikaball/sprites.hpp>
#include <array>

namespace pika::view {

MenuView::MenuView(SDL_Renderer *renderer, SDL_Texture *sprite_sheet) :
  View(renderer, sprite_sheet)
{}

GameState MenuView::update() {
  if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
    return GameState::Menu;
  }

  // Fill the background white or black
  if (selection_ == MenuPlayerSelection::SINGLE_PLAYER) {
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
  }
  else {
    SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
  }
  SDL_RenderClear(renderer_);

  // TODO: Render the background and messages
  render_background();
  render_fight_msg();

  SDL_RenderPresent(renderer_);

  frame_counter_++;
  if (frame_counter_ < start_frames && input_.enter) {
    // The animation is skipped by pressing enter
    frame_counter_ = start_frames;
    return GameState::Menu;
  }
  // On the first frames, just render the animation (skip input processing).
  if (frame_counter_ <= start_frames) {
    return GameState::Menu;
  }

  // Process input to update the game mode selection
  if (selection_ == MenuPlayerSelection::SINGLE_PLAYER && input_.down) {
    selection_ = MenuPlayerSelection::MULTI_PLAYER;
  }
  else if (selection_ == MenuPlayerSelection::MULTI_PLAYER && input_.up) {
    selection_ = MenuPlayerSelection::SINGLE_PLAYER;
  }

  // Process input to check if the game must start
  if (input_.enter) {
    return GameState::Round;
  }

  return GameState::Menu;
}

void MenuView::start() {
  frame_counter_ = 0;
  input_ = {};
  selection_ = MenuPlayerSelection::SINGLE_PLAYER;
  background_offset_ = 0;
  background_alpha_ = 0.0;
  if (!background_texture_) {
    preload_background();
  }
}

void MenuView::set_input(const MenuInput& input) {
  input_ = input;
}

void MenuView::render_background() {
  constexpr auto sprite_width = static_cast<int>(sprite::sitting_pikachu.w);
  background_offset_ = (background_offset_ + 2) % sprite_width;

  SDL_FRect f_src;
  const SDL_Rect src_rect {
    .x = background_offset_,
    .y = background_offset_,
    .w = screen_width,
    .h = screen_height,
  };

  // Handle background alpha based on frame counter
  if (frame_counter_ >= start_frames) {
    background_alpha_ = 1.0;
  }
  else if (frame_counter_ > 30) {
    background_alpha_ = std::min(1.0f, background_alpha_ + 0.04f);
  }
  SDL_SetTextureAlphaModFloat(background_texture_.get(), background_alpha_);

  SDL_RectToFRect(&src_rect, &f_src);
  SDL_RenderTexture(
    renderer_, background_texture_.get(), &f_src, nullptr);
}

void MenuView::render_fight_msg() const {
  static constexpr std::array fight_msg_sizes = {20, 22, 25, 27, 30, 27, 25, 22, 20};
  constexpr auto sprite_width = static_cast<int>(sprite::messages_fight.w);
  constexpr auto sprite_height = static_cast<int>(sprite::messages_fight.h);

  // The sprite grows for the first 30 frames, then it resizes based on the
  const int size = (frame_counter_ < 30) ? static_cast<int>(frame_counter_)
                                         : fight_msg_sizes[(frame_counter_ + 1) % 9];
  const int h_width = size * sprite_width / 30 / 2;
  const int h_height = size * sprite_height / 30 / 2;
  SDL_FRect f_dst;
  const SDL_Rect dst_rect {
    .x = 100 - h_width,
    .y = 70 - h_height,
    .w = h_width * 2,
    .h = h_height * 2,
  };
  SDL_RectToFRect(&dst_rect, &f_dst);
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::messages_fight, &f_dst);
}


void MenuView::preload_background() {
  if (sprite_sheet_ == nullptr || renderer_ == nullptr) {
    return;
  }

  // Compute the number of sprite rows and cols with some extra size to allow movement
  constexpr auto sprite_width = static_cast<int>(sprite::sitting_pikachu.w);
  constexpr auto sprite_height = static_cast<int>(sprite::sitting_pikachu.h);
  constexpr int num_cols = screen_width / sprite_width + 2;
  constexpr int num_rows = screen_height / sprite_height + 2;

  // Create a target texture to render all the background elements to
  background_texture_.reset(SDL_CreateTexture(
    renderer_,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_TARGET,
    num_cols * sprite_width,
    num_rows * sprite_height
  ));

  // Focus the renderer on the target texture
  SDL_SetRenderTarget(renderer_, background_texture_.get());

  // Fill the background green
  SDL_SetRenderDrawColor(renderer_, 0x00, 0xFF, 0x0, 0xFF);
  SDL_RenderClear(renderer_);

  // Build the sky
  SDL_FRect f_dst;
  SDL_Rect dst = {
    // Preset width and height with pikachu sprite size
    .w = sprite_width,
    .h = sprite_height,
  };
  for (int i = 0; i < num_cols; i++) {
    for (int j = 0; j < num_rows; j++) {
      // Set position and render texture
      dst.x = i * sprite_width;
      dst.y = j * sprite_height;
      SDL_RectToFRect(&dst, &f_dst);
      SDL_RenderTexture(
        renderer_, sprite_sheet_, &sprite::sitting_pikachu, &f_dst);
    }
  }

  // Set the render target back to the main window
  SDL_SetRenderTarget(renderer_, nullptr);
}

} // namespace pika::view