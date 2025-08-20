#ifndef PIKA_OPTIONS_VIEW_HPP
#define PIKA_OPTIONS_VIEW_HPP

#include "SDL3_ttf/SDL_ttf.h"
#include "pikaball/common.hpp"
#include "pikaball/sprites.hpp"
#include "view.hpp"

namespace pika::view {

class OptionsView final : public View {

public:
// Total number of frames to render the intro
// constexpr static unsigned int max_frames = 165;

private:
// Fixed sprite positions
constexpr static SDL_FRect mlp_dst {
  .x = screen_h_width + 15,
  .y = screen_h_height - sprite::msg_init_mark_mlp.h / 2,
  .w = sprite::msg_init_mark_mlp.w,
  .h = sprite::msg_init_mark_mlp.h,
};

constexpr static SDL_FRect background_dst {
  .x = screen_h_width - 150,
  .y = screen_h_height - 145,
  .w = 300,
  .h = 290,
};

constexpr static std::string text_1 = "Options";
constexpr static SDL_Color text_color = { 255, 255, 255 };
constexpr static SDL_FRect text_dst {
  .x = background_dst.x + 20,
  .y = background_dst.y + 50,
  .w = 80,
  .h = 20,
};

public:
  ~OptionsView() override = default;
  OptionsView(OptionsView const&) = delete;
  OptionsView(OptionsView &&) = delete;
  OptionsView &operator=(OptionsView const&) = delete;
  OptionsView &operator=(OptionsView &&) = delete;

  explicit OptionsView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet, TTF_Font* text_font) :
    View(renderer, sprite_sheet),
    text_font_(text_font)
  {
    start();
  }

  /**
   * Start the view
   * Set the frame counter to zero and the player selection to 1P.
   */
  void start() override {
    black_fade_alpha_ = 0.0;
    if (!background_texture_) {
      preload_textures();
    }
  }

  /** Render the options text */
  void render() const {
    if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
      return;
    }

    // Fill the background gray
    // TODO: Remove to show the game as background. Enable for debug.
    // SDL_SetRenderDrawColor(renderer_, 0xAA, 0xAA, 0xAA, 0xFF);
    // SDL_RenderClear(renderer_);

    // Render background
    SDL_SetTextureAlphaModFloat(background_texture_.get(), 0.9);
    SDL_RenderTexture(renderer_, background_texture_.get(), nullptr, &background_dst);

    // Render text
    SDL_RenderTexture(renderer_, text_texture_.get(), nullptr, &text_dst);
  }

private:
  // Background black cover
  SDL_Texture_ptr background_texture_ {nullptr, SDL_DestroyTexture};
  // Text textures
  SDL_Texture_ptr text_texture_ {nullptr, SDL_DestroyTexture};
  // Pointer to text font
  TTF_Font* text_font_ {nullptr};

  void preload_textures() {
    if (renderer_ == nullptr) {
      return;
    }

    // Create a small black surface for the menu background
    background_texture_.reset(SDL_CreateTexture(
      renderer_,
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_TARGET,
      background_dst.w,
      background_dst.h
    ));
    // Set the texture scaling mode to nearest interpolation
    SDL_SetTextureScaleMode(background_texture_.get(), SDL_SCALEMODE_NEAREST);
    // Clear the black texture with a black background
    SDL_SetRenderTarget(renderer_, background_texture_.get());
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer_);

    // Set the render target back to the main window
    SDL_SetRenderTarget(renderer_, nullptr);

    // Load text textures
    // Load the text into a temporary surface
    SDL_Surface* text_surface = TTF_RenderText_Solid(text_font_, text_1.c_str(), text_1.size(), text_color);
    if (text_surface == nullptr) {
      SDL_Log( "Unable to load text %s! SDL Error: %s\n", sprite_sheet_filename, SDL_GetError());
      throw std::runtime_error("Failed to load text!");
    }
    // Generate the texture and save it
    text_texture_.reset(SDL_CreateTextureFromSurface(renderer_, text_surface));
    // Release the temporary surface object
    SDL_DestroySurface(text_surface);
  }
};

} // namespace pika::view

#endif // PIKA_OPTIONS_VIEW_HPP
