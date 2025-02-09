#include "menu_view.hpp"

#include <array>
#include <pikaball/common.hpp>
#include <pikaball/sprites.hpp>

namespace pika::view {

MenuView::MenuView(SDL_Renderer *renderer, SDL_Texture *sprite_sheet) :
  View(renderer, sprite_sheet)
{}

GameState MenuView::update() {
  if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
    return GameState::Menu;
  }

  render();

  frame_counter_++;
  switch (state_) {
  case MenuState::Menu:
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
      selection_size_ = 2;
    }
    else if (selection_ == MenuPlayerSelection::MULTI_PLAYER && input_.up) {
      selection_ = MenuPlayerSelection::SINGLE_PLAYER;
      selection_size_ = 2;
    }

    // Process input to check if the game must start
    if (input_.enter) {
      state_ = MenuState::FadeOut;
    }
    break;
  case MenuState::FadeOut:
    // After fading out completely, change game state to start the game
    if (black_fade_alpha_ >= 1.0) {
      return GameState::Round;
    }
    break;
  }

  return GameState::Menu;
}

void MenuView::start() {
  state_ = MenuState::Menu;
  frame_counter_ = 0;
  black_fade_alpha_ = 0.0;
  input_ = {};
  selection_ = MenuPlayerSelection::SINGLE_PLAYER;
  background_offset_ = 0;
  pika_background_alpha_ = 0.0;
  copyright_alpha_ = 0.0;
  selection_size_ = 2;
  if (!background_texture_ || !copyright_texture_) {
    preload_background();
  }
}

void MenuView::set_input(const MenuInput& input) {
  input_ = input;
}

void MenuView::render() {
  // Fill the background black
  SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer_);

  // Render the background and messages
  render_background();
  render_fight_msg();
  render_copyright_msg();
  render_title_msgs();
  render_player_selection_msg();

  if (state_ == MenuState::FadeOut) {
    fade_out(1.0f / 30);
  }
  SDL_RenderPresent(renderer_);
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
    pika_background_alpha_ = 1.0;
  }
  else if (frame_counter_ > 30) {
    pika_background_alpha_ = std::min(1.0f, pika_background_alpha_ + 0.04f);
  }
  SDL_SetTextureAlphaModFloat(background_texture_.get(), pika_background_alpha_);

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

void MenuView::render_copyright_msg() {
  // Handle background alpha based on frame counter
  if (frame_counter_ >= start_frames) {
    copyright_alpha_ = 1.0;
  }
  else {
    copyright_alpha_ = std::min(1.0f, copyright_alpha_ + 0.04f);
  }
  SDL_SetTextureAlphaModFloat(copyright_texture_.get(), copyright_alpha_);

  constexpr SDL_FRect dst_rect {
    .x = screen_h_width - sprite::messages_copyright.w / 2,
    .y = 264,
    .w = sprite::messages_copyright.w,
    .h = sprite::messages_copyright.h,
  };
  SDL_RenderTexture(
    renderer_, copyright_texture_.get(), nullptr, &dst_rect);
}

void MenuView::render_title_msgs() const {
  if (frame_counter_ < 30) {
    return;
  }
  constexpr auto sprite_width = static_cast<int>(sprite::messages_pikachu_volleyball.w);
  constexpr auto sprite_height = static_cast<int>(sprite::messages_pikachu_volleyball.h);

  SDL_FRect f_title_dst;
  SDL_Rect title_dst {
    .x = 140,
    .y = 80,
    .w = sprite_width,
    .h = sprite_height,
  };
  if (frame_counter_ <= 44) {
    const int x_diff = 195 - 15 * (static_cast<int>(frame_counter_) - 30);
    title_dst.x = 140 + x_diff;
  }
  else if (frame_counter_ <= 55) {
    title_dst.w = 200 - 15 * (static_cast<int>(frame_counter_) - 44);
  }
  else if (frame_counter_ <= start_frames) {  // <= 71
    title_dst.w = 40 + 15 * (static_cast<int>(frame_counter_) - 55);
  }
  SDL_RectToFRect(&title_dst, &f_title_dst);
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::messages_pikachu_volleyball, &f_title_dst);

  // Pikachu tournament message
  constexpr SDL_FRect tournament_dst {
    .x = 170,
    .y = 40,
    .w = sprite::messages_pokemon_tournament.w,
    .h = sprite::messages_pokemon_tournament.h,
  };
  if (frame_counter_ > start_frames) {
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::messages_pokemon_tournament, &tournament_dst);
  }
}

void MenuView::render_player_selection_msg() {
  if (frame_counter_ < start_frames) {
    return;
  }
  constexpr auto sprite_width = static_cast<int>(sprite::messages_player_1.w);
  constexpr auto sprite_height = static_cast<int>(sprite::messages_player_1.h);

  if (selection_size_ < 10) {
    selection_size_++;
  }

  SDL_FRect f_dst;
  SDL_Rect p1_dst {
    .x = screen_h_width - sprite_width / 2,
    .y = 184,
    .w = sprite_width,
    .h = sprite_height,
  };
  SDL_Rect p2_dst {
    .x = screen_h_width - sprite_width / 2,
    .y = 184 + 30,
    .w = sprite_width,
    .h = sprite_height,
  };
  // Adjust the size of the selected option
  auto &[sel_x, sel_y, sel_w, sel_h] =
    (selection_ == MenuPlayerSelection::MULTI_PLAYER) ? p2_dst : p1_dst;
  const int h_width_increment = static_cast<int>(selection_size_) + 2;
  const int h_height_increment = static_cast<int>(selection_size_);
  sel_x -= h_width_increment;
  sel_y -= h_height_increment;
  sel_w += 2 * h_width_increment;
  sel_h += 2 * h_height_increment;

  SDL_RectToFRect(&p1_dst, &f_dst);
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::messages_player_1, &f_dst);
  SDL_RectToFRect(&p2_dst, &f_dst);
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::messages_player_2, &f_dst);

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
  // Set the texture scaling mode to nearest interpolation
  SDL_SetTextureScaleMode(background_texture_.get(), SDL_SCALEMODE_NEAREST);

  // Focus the renderer on the target texture
  SDL_SetRenderTarget(renderer_, background_texture_.get());

  // Fill the background green
  SDL_SetRenderDrawColor(renderer_, 0x00, 0xFF, 0x00, 0xFF);
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

  // Then, change the target to the copyright texture
  copyright_texture_.reset(SDL_CreateTexture(
    renderer_,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_TARGET,
    sprite::messages_copyright.w,
    sprite::messages_copyright.h
  ));
  // Set the texture scaling mode to nearest interpolation
  SDL_SetTextureScaleMode(copyright_texture_.get(), SDL_SCALEMODE_NEAREST);

  // Copy the texture from the sheet
  SDL_SetRenderTarget(renderer_, copyright_texture_.get());
  SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::messages_copyright, nullptr);


  // Set the render target back to the main window
  SDL_SetRenderTarget(renderer_, nullptr);
}

} // namespace pika::view