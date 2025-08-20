#ifndef PIKA_OPTIONS_VIEW_HPP
#define PIKA_OPTIONS_VIEW_HPP

#include <algorithm>
#include <ranges>

#include "SDL3_ttf/SDL_ttf.h"
#include "pikaball/common.hpp"
#include "pikaball/sprites.hpp"
#include "view.hpp"

namespace pika::view {

[[nodiscard]] inline View::SDL_Texture_ptr load_text_texture(
  SDL_Renderer* renderer,
  TTF_Font* text_font,
  const std::string & text,
  const SDL_Color & text_color = {255, 255, 255})
{
  // Load the text into a temporary surface
  SDL_Surface* text_surface =
    TTF_RenderText_Solid(text_font, text.c_str(), 0, text_color);
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

class OptionItem {
public:
  virtual ~OptionItem() = default;
  OptionItem(OptionItem const&) = delete;
  OptionItem(OptionItem &&) = delete;
  OptionItem &operator=(OptionItem const&) = delete;
  OptionItem &operator=(OptionItem &&) = delete;

  /**
   * Create a new OptionItem
   * @param renderer A non-owning pointer to the SDL renderer
   * @param text_font The TTF font used to generate the text textures
   * @param name The name for this option
   * @param y_position The vertical position to render this option (topmost)
   */
  explicit OptionItem(
    SDL_Renderer* renderer,
    TTF_Font* text_font,
    const std::string &name,
    const int y_position
  )
  : renderer_(renderer),
    text_font_(text_font),
    y_position_(y_position)
  {
    name_texture_ = load_text_texture(renderer, text_font, name);
    // Initialize name render position
    constexpr int name_h = 20;
    const int name_w = name_texture_->w * name_h / 40;
    const int name_x = static_cast<int>(screen_h_width - name_w / 2);
    name_dst_.x = static_cast<float>(name_x);
    name_dst_.y = static_cast<float>(y_position_);
    name_dst_.w = static_cast<float>(name_w);
    name_dst_.h = name_h;
  }

  /** * Select the next / previous option values. */
  virtual void select_option(int option) = 0;

  /**
   * Render the OptionItem values.
   * The selected value must be bigger and with a different color
   */
  virtual void render_values() = 0;

  /**
   * Render the Option name (title) and the option values
   */
  void render() {
    SDL_RenderTexture(renderer_, name_texture_.get(), nullptr, &name_dst_);

    // Render option values
    render_values();
  }

  /**
   * Get the current position where the title is rendered
   * @return An SDL_FRect with the position and size of the rendered name
   */
  [[nodiscard]] SDL_FRect get_position() const {
    return name_dst_;
  }

  /**
   * Get the current position where the title is rendered
   * @return An SDL_FRect with the position and size of the rendered name
   */
  [[nodiscard]] int get_selection() const {
    return selected_;
  }

protected:
  // Convenient type alias to hold owning textures
  using SDL_Texture_ptr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
  SDL_Renderer* renderer_;
  TTF_Font* text_font_;
  SDL_Texture_ptr name_texture_ {nullptr, SDL_DestroyTexture};
  const int y_position_ = 0;
  SDL_FRect name_dst_ {};
  int selected_ = 0;
};

class OptionItemList final : public OptionItem {
public:
  ~OptionItemList() override = default;
  OptionItemList(OptionItemList const&) = delete;
  OptionItemList(OptionItemList &&) = delete;
  OptionItemList &operator=(OptionItemList const&) = delete;
  OptionItemList &operator=(OptionItemList &&) = delete;

  explicit OptionItemList(
    SDL_Renderer* renderer,
    TTF_Font* text_font,
    const std::string &name,
    const int y_position
  )
  :  OptionItem(renderer, text_font, name, y_position)
  {}

  explicit OptionItemList(
    SDL_Renderer* renderer,
    TTF_Font* text_font,
    const std::string &name,
    const int y_position,
    const std::initializer_list<std::string> options
  )
  : OptionItem(renderer, text_font, name, y_position),
    options_(options)
  {
    for (const auto& option : options_) {
      opt_textures_.emplace_back(load_text_texture(renderer_, text_font_, option));
      opt_select_textures_.emplace_back(
        load_text_texture(renderer_, text_font_, option, {255, 0, 0})
      );
    }
  }

  /**
   * Add a new option value to the list
   * @param option the option value to be added
   */
  void add_option(const std::string & option) {
    options_.emplace_back(option);
    opt_textures_.emplace_back(load_text_texture(renderer_, text_font_, option));
    opt_select_textures_.emplace_back(load_text_texture(renderer_, text_font_, option, {255, 0, 0}));
  }

  void select_option(const int option) override {
    selected_ = std::clamp(option, 0, static_cast<int>(options_.size() - 1));
  }

  void render_values() override {
    constexpr int extra_space = 15;
    constexpr int opt_h = 15;
    constexpr float scale_factor = opt_h / 40.f;
    // Text textures are scaled to opt_h (from 40 pixels of initial height)
    size_t total_width = extra_space * (options_.size() - 1);
    for (int i = 0; i < options_.size(); i++) {
      const auto opt_w = static_cast<float>(opt_textures_[i]->w);
      total_width += static_cast<size_t>(opt_w * scale_factor);
    }

    // Start rendering here, and advance this cursor after rendering options
    int cur_render_x = static_cast<int>(screen_h_width - total_width / 2);

    for (int i = 0; i < options_.size(); i++) {
      SDL_Texture* text_texture = nullptr;
      if (i == selected_) {
        // Render the red text for this option
        text_texture = opt_select_textures_[i].get();
      } else {
        text_texture = opt_textures_[i].get();
      }

      const int opt_w = opt_textures_[i]->w * opt_h / 40;
      const SDL_FRect opt_dst {
        .x = static_cast<float>(cur_render_x),
        .y = static_cast<float>(y_position_ + 30),
        .w = static_cast<float>(opt_w),
        .h = opt_h,
      };
      SDL_RenderTexture(renderer_, text_texture, nullptr, &opt_dst);

      // Advance the render cursor and add some extra space
      cur_render_x += opt_w + extra_space;
    }
  }

private:
  std::vector<std::string> options_;
  std::vector<SDL_Texture_ptr> opt_textures_;
  std::vector<SDL_Texture_ptr> opt_select_textures_;
};

class OptionsView final : public View {

constexpr static SDL_FRect background_dst {
  .x = screen_h_width - 150,
  .y = screen_h_height - 145,
  .w = 300,
  .h = 290,
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
    preload_textures();

    // Create option items
    auto speed_options = std::make_unique<OptionItemList>(
      renderer_, text_font_, txt::str_opt_speed, background_dst.y + 75
    );
    speed_options->add_option(txt::str_slow);
    speed_options->add_option(txt::str_medium);
    speed_options->add_option(txt::str_fast);
    options_[OptionMenuSelection::Speed] = std::move(speed_options);

    auto points_options = std::make_unique<OptionItemList>(
      renderer_, text_font_, txt::str_opt_points, background_dst.y + 140
    );
    points_options->add_option(txt::str_5_pts);
    points_options->add_option(txt::str_10_pts);
    points_options->add_option(txt::str_15_pts);
    options_[OptionMenuSelection::Points] = std::move(points_options);

    auto music_options = std::make_unique<OptionItemList>(
      renderer_, text_font_, txt::str_opt_music, background_dst.y + 205
    );
    music_options->add_option(txt::str_on);
    music_options->add_option(txt::str_off);
    options_[OptionMenuSelection::Music] = std::move(music_options);

    start();
  }

  /**
   * Start the view
   * Set the frame counter to zero and the player selection to 1P.
   */
  void start() override {
    black_fade_alpha_ = 0.0;
    selection_ = OptionMenuSelection::Speed;
  }

  void select_option(const OptionMenuSelection selection) {
    selection_ = selection;
  }

  void select_speed_option(const SpeedOptionSelection selection) {
    options_[OptionMenuSelection::Speed]->select_option(static_cast<int>(selection));
  }

  void select_points_option(const PointsOptionSelection selection) {
    options_[OptionMenuSelection::Points]->select_option(static_cast<int>(selection));
  }

  void select_music_option(const OnOffSelection selection) {
    options_[OptionMenuSelection::Music]->select_option(static_cast<int>(selection));
  }

  /** Render the options text */
  void render() const {
    if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
      return;
    }

    // Render background
    SDL_SetTextureAlphaModFloat(background_texture_.get(), 0.92);
    SDL_RenderTexture(renderer_, background_texture_.get(), nullptr, &background_dst);

    // Render title
    constexpr float title_h = 25;
    constexpr float title_w = 262 * title_h / 40;
    constexpr SDL_FRect title_dst {
      .x = screen_h_width - title_w / 2,
      .y = background_dst.y + 20,
      .w = title_w,
      .h = title_h,
    };
    SDL_RenderTexture(renderer_, title_texture_.get(), nullptr, &title_dst);

    for (const auto &option : options_ | std::views::values) {
      option->render();
    }

    // Render pokeball at the selected option item
    const SDL_FRect& selected_option_dst = options_.at(selection_)->get_position();
    constexpr float opt_ball_w = 20;
    constexpr float opt_ball_h = 20;
    const SDL_FRect opt_ball_dst {
      .x = selected_option_dst.x - opt_ball_w - 10,
      .y = selected_option_dst.y,
      .w = opt_ball_w,
      .h = opt_ball_h,
    };
    SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::ball_0, &opt_ball_dst);
  }

private:
  // Background black cover
  SDL_Texture_ptr background_texture_ {nullptr, SDL_DestroyTexture};
  // Pointer to text font
  TTF_Font* text_font_ {nullptr};
  // Title text textures
  SDL_Texture_ptr title_texture_ {nullptr, SDL_DestroyTexture};
  // Option items
  std::map<OptionMenuSelection, std::unique_ptr<OptionItem>> options_;
  OptionMenuSelection selection_ = OptionMenuSelection::Speed;

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
    title_texture_ = load_text_texture(renderer_, text_font_, txt::str_options);
  }

};

} // namespace pika::view

#endif // PIKA_OPTIONS_VIEW_HPP
