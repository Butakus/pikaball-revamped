#ifndef PIKA_FPS_VIEW_HPP
#define PIKA_FPS_VIEW_HPP

#include <format>
#include <string>

#include "SDL3_ttf/SDL_ttf.h"
#include "pikaball/common.hpp"
#include "text_texture.hpp"
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
  .y = background_dst.y,
  .w = 32,
  .h = background_dst.h,
};

// FPS letters ("FPS") position
constexpr static SDL_FRect fps_txt_dst {
  .x = fps_dst.x + fps_dst.w + 3,
  .y = background_dst.y,
  .w = 22,
  .h = background_dst.h,
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


  /**
   * Render the FPS text.
   * Called every frame, so it must not throw: if the FPS value cannot be rendered,
   * the value is skipped and a warning is logged once.
   */
  void render(const float fps) {
    if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
      return;
    }

    // Render background with 50% opacity
    render_black_rect(0.50f, &background_dst);

    // Render FPS value
    update_fps_texture(std::format("{:.1f}", fps));
    if (fps_texture_) {
      SDL_RenderTexture(renderer_, fps_texture_.get(), nullptr, &fps_dst);
    }

    // Render the FPS letters
    SDL_RenderTexture(renderer_, fps_txt_texture_.get(), nullptr, &fps_txt_dst);
  }

private:
  SDL_Texture_ptr fps_txt_texture_ {nullptr, SDL_DestroyTexture};
  // Cached texture of the FPS value, and the text it was rendered from
  SDL_Texture_ptr fps_texture_ {nullptr, SDL_DestroyTexture};
  std::string fps_str_;
  // Pointer to text font
  TTF_Font* text_font_ {nullptr};
  // Set after the first failure to render the FPS value, to avoid logging it every frame
  bool fps_text_error_logged_ {false};

  /** Re-render the FPS value texture if the text changed. */
  void update_fps_texture(const std::string& fps_str) {
    // Only re-create the text texture when the displayed value changes
    if (fps_str == fps_str_) {
      return;
    }
    fps_str_ = fps_str;
    fps_texture_ = render_text_texture(renderer_, text_font_, fps_str_);
    if (!fps_texture_ && !fps_text_error_logged_) {
      SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Failed to render FPS value: %s", SDL_GetError());
      fps_text_error_logged_ = true;
    }
  }

  void preload_textures() {
    if (renderer_ == nullptr) {
      return;
    }

    // Load text textures
    fps_txt_texture_ = load_text_texture(renderer_, text_font_, "FPS");
  }
};

} // namespace pika::view

#endif // PIKA_FPS_VIEW_HPP
