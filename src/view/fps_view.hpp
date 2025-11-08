#ifndef PIKA_FPS_VIEW_HPP
#define PIKA_FPS_VIEW_HPP

#include <format>

#include "SDL3_ttf/SDL_ttf.h"
#include "pikaball/common.hpp"
#include "view.hpp"

namespace pika::view {


class FPSView final : public View {

// Black background
constexpr static SDL_FRect background_dst {
  .x = screen_h_width - 30,
  .y = 2,
  .w = 60,
  .h = 20,
};

// FPS value (digits) position
constexpr static SDL_FRect fps_dst {
  .x = background_dst.x + 2,
  .y = background_dst.y + 2,
  .w = 32,
  .h = background_dst.h - 2,
};

// FPS letters ("FPS") position
constexpr static SDL_FRect fps_txt_dst {
  .x = fps_dst.x + fps_dst.w + 3,
  .y = background_dst.y + 2,
  .w = 22,
  .h = background_dst.h - 2,
};

public:
  ~FPSView() override = default;
  FPSView(FPSView const&) = delete;
  FPSView(FPSView &&) = delete;
  FPSView &operator=(FPSView const&) = delete;
  FPSView &operator=(FPSView &&) = delete;

  explicit FPSView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet, TTF_Font* text_font) :
    View(renderer, sprite_sheet),
    text_font_(text_font)
  {
    preload_textures();
  }


  /** Render the FPS text */
  void render(const float fps) const {
    if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
      return;
    }

    // Render background with 50% opacity
    SDL_SetTextureAlphaModFloat(background_texture_.get(), 0.50);
    SDL_RenderTexture(renderer_, background_texture_.get(), nullptr, &background_dst);

    // Render FPS value
    const std::string fps_str = std::format("{:.1f}", fps);
    const auto fps_texture = FPSView::load_text_texture(renderer_, text_font_, fps_str);
    SDL_RenderTexture(renderer_, fps_texture.get(), nullptr, &fps_dst);

    // Render the FPS letters
    SDL_RenderTexture(renderer_, fps_txt_texture_.get(), nullptr, &fps_txt_dst);
  }

private:
  // Background black cover
  SDL_Texture_ptr background_texture_ {nullptr, SDL_DestroyTexture};
  SDL_Texture_ptr fps_txt_texture_ {nullptr, SDL_DestroyTexture};
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
    fps_txt_texture_ = FPSView::load_text_texture(renderer_, text_font_, "FPS");
    // title_texture_ = load_text_texture(renderer_, text_font_, txt::str_options);
  }


  [[nodiscard]] static SDL_Texture_ptr load_text_texture(
    SDL_Renderer* renderer,
    TTF_Font* text_font,
    const std::string & text,
    const SDL_Color & text_color = {255, 255, 255})
  {
    // Load the text into a temporary surface
    SDL_Surface* text_surface = TTF_RenderText_Solid(text_font, text.c_str(), 0, text_color);
    if (text_surface == nullptr) {
      SDL_Log("Unable to load text! SDL Error: %s\n", SDL_GetError());
      throw std::runtime_error("Failed to load text!");
    }
    // Generate the texture and save it
    View::SDL_Texture_ptr text_texture {nullptr, SDL_DestroyTexture};
    text_texture.reset(SDL_CreateTextureFromSurface(renderer, text_surface));
    // Release the temporary surface object
    SDL_DestroySurface(text_surface);

    return text_texture;
  }

};

} // namespace pika::view

#endif // PIKA_FPS_VIEW_HPP
