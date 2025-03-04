#ifndef PIKA_MENU_VIEW_HPP
#define PIKA_MENU_VIEW_HPP

#include "view.hpp"
#include <pikaball/sprites.hpp>

namespace pika::view {

class MenuView final : public View {
public:
  // Number of frames at the start to block the input
  constexpr static unsigned int start_frames = 71;
  // Number of frames for the final fade out effect
  constexpr static unsigned int fade_out_frames = 30;

  ~MenuView() override = default;
  MenuView(MenuView const&) = delete;
  MenuView(MenuView &&) = delete;
  MenuView &operator=(MenuView const&) = delete;
  MenuView &operator=(MenuView &&) = delete;

  explicit MenuView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet) :
    View(renderer, sprite_sheet)
  {}

  /**
   * Start the view
   * Set the frame counter to zero and the player selection to 1P.
   */
  void start() override {
    state_ = MenuState::Menu;
    black_fade_alpha_ = 0.0;
    // TODO: Warning: This may be different from game
    selection_ = MenuPlayerSelection::SINGLE_PLAYER;
    background_offset_ = 0;
    pika_background_alpha_ = 0.0;
    copyright_alpha_ = 0.0;
    selection_size_ = 2;
    if (!background_texture_ || !copyright_texture_) {
      preload_background();
    }
  }

  /**
   * Render the new frame
   * @param frame_counter The current frame counter (used for animations)
   */
  void render(const unsigned int frame_counter) {
    if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
      return;
    }

    // Fill the background black
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer_);

    // Render the background and messages
    render_background(frame_counter);
    render_fight_msg(frame_counter);
    render_copyright_msg(frame_counter);
    render_title_msgs(frame_counter);
    render_player_selection_msg(frame_counter);

    if (state_ == MenuState::FadeOut) {
      fade_out(1.0f / fade_out_frames);
    }
  }

  /**
   *Change the selected game mode.
   * Reset the selection size after the change to do the resize animation
   * @param selection the new selected game mode
   */
  void change_selection(const MenuPlayerSelection selection) {
    selection_size_ = 2;
    selection_ = selection;
  }

  /**
   * Change the menu state (Menu / FadeOut)
   * @param state The new menu state
   */
  void set_state(const MenuState state) {
    state_ = state;
  }

private:
  MenuState state_ {MenuState::Menu};
  MenuPlayerSelection selection_ {MenuPlayerSelection::SINGLE_PLAYER};
  // Background with the repeating sitting pikachus
  SDL_Texture_ptr background_texture_{nullptr, SDL_DestroyTexture};
  // Copyright message. Needs its own texture to apply an independent alpha.
  SDL_Texture_ptr copyright_texture_{nullptr, SDL_DestroyTexture};
  // Offset for the sitting pikachu background sprites
  int background_offset_ {0};
  float pika_background_alpha_ {0.0};
  float copyright_alpha_ {0.0};
  unsigned int selection_size_ {2};

  /**** Private methods ****/

  /**
   * Render the background: Sitting pikachu moving diagonally.
   * @param frame_counter The current frame counter (used for animations)
   */
  void render_background(const unsigned int frame_counter) {
    constexpr int sprite_width = static_cast<int>(sprite::sitting_pikachu.w);
    background_offset_ = (background_offset_ + 2) % sprite_width;

    const SDL_FRect src_rect {
      .x = static_cast<float>(background_offset_),
      .y = static_cast<float>(background_offset_),
      .w = screen_width,
      .h = screen_height,
    };

    // Handle background alpha based on frame counter
    if (frame_counter >= start_frames) {
      pika_background_alpha_ = 1.0;
    }
    else if (frame_counter > 30) {
      pika_background_alpha_ = std::min(1.0f, pika_background_alpha_ + 0.04f);
    }
    SDL_SetTextureAlphaModFloat(background_texture_.get(), pika_background_alpha_);

    SDL_RenderTexture(
      renderer_, background_texture_.get(), &src_rect, nullptr);
  }

  /**
   * Render the "fight!" / "MLP powah!" sprite
   * @param frame_counter The current frame counter (used for animations)
   */
  void render_fight_msg(const unsigned int frame_counter) const {
    static constexpr std::array fight_msg_sizes = {20, 22, 25, 27, 30, 27, 25, 22, 20};
    constexpr int sprite_width = static_cast<int>(sprite::msg_fight.w);
    constexpr int sprite_height = static_cast<int>(sprite::msg_fight.h);

    // The sprite grows for the first 30 frames, then it resizes based on the
    const int size = frame_counter < 30 ? static_cast<int>(frame_counter)
                                        : fight_msg_sizes[(frame_counter + 1) % 9];
    const int h_width = size * sprite_width / 30 / 2;
    const int h_height = size * sprite_height / 30 / 2;
    const SDL_FRect dst_rect {
      .x = static_cast<float>(100 - h_width),
      .y = static_cast<float>(70 - h_height),
      .w = static_cast<float>(h_width * 2),
      .h = static_cast<float>(h_height * 2),
    };
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::msg_fight, &dst_rect);
  }

  /**
   * Render the copyright message at the bottom
   * @param frame_counter The current frame counter (used for animations)
   */
  void render_copyright_msg(const unsigned int frame_counter) {
    // Handle background alpha based on frame counter
    if (frame_counter >= start_frames) {
      copyright_alpha_ = 1.0;
    }
    else {
      copyright_alpha_ = std::min(1.0f, copyright_alpha_ + 0.04f);
    }
    SDL_SetTextureAlphaModFloat(copyright_texture_.get(), copyright_alpha_);

    static constexpr SDL_FRect dst_rect {
      .x = screen_h_width - sprite::msg_copyright.w / 2,
      .y = 264,
      .w = sprite::msg_copyright.w,
      .h = sprite::msg_copyright.h,
    };
    SDL_RenderTexture(
      renderer_, copyright_texture_.get(), nullptr, &dst_rect);
  }

  /**
   * Render the title sprites (Pikachu Volleyball / Tournament)
   * @param frame_counter The current frame counter (used for animations)
   */
  void render_title_msgs(const unsigned int frame_counter) const {
    if (frame_counter < 30) {
      return;
    }
    constexpr int sprite_width = static_cast<int>(sprite::msg_pikachu_volleyball.w);
    constexpr int sprite_height = static_cast<int>(sprite::msg_pikachu_volleyball.h);

    SDL_FRect f_title_dst;
    SDL_Rect title_dst {
      .x = 140,
      .y = 80,
      .w = sprite_width,
      .h = sprite_height,
    };
    if (frame_counter <= 44) {
      const int x_diff = 195 - 15 * (static_cast<int>(frame_counter) - 30);
      title_dst.x = 140 + x_diff;
    }
    else if (frame_counter <= 55) {
      title_dst.w = 200 - 15 * (static_cast<int>(frame_counter) - 44);
    }
    else if (frame_counter <= start_frames) {  // <= 71
      title_dst.w = 40 + 15 * (static_cast<int>(frame_counter) - 55);
    }
    SDL_RectToFRect(&title_dst, &f_title_dst);
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::msg_pikachu_volleyball, &f_title_dst);

    // Pikachu tournament message
    static constexpr SDL_FRect tournament_dst {
      .x = 170,
      .y = 40,
      .w = sprite::msg_pokemon_tournament.w,
      .h = sprite::msg_pokemon_tournament.h,
    };
    if (frame_counter > start_frames) {
      SDL_RenderTexture(
        renderer_, sprite_sheet_, &sprite::msg_pokemon_tournament, &tournament_dst);
    }
  }

  /**
   * Render 1P/2P selection messages
   * @param frame_counter The current frame counter (used for animations)
   */
  void render_player_selection_msg(const unsigned int frame_counter) {
    if (frame_counter < start_frames) {
      return;
    }
    constexpr int sprite_width = static_cast<int>(sprite::msg_player_1.w);
    constexpr int sprite_height = static_cast<int>(sprite::msg_player_1.h);

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
    // Take references to the Rect of the current selection
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
      renderer_, sprite_sheet_, &sprite::msg_player_1, &f_dst);
    SDL_RectToFRect(&p2_dst, &f_dst);
    SDL_RenderTexture(
      renderer_, sprite_sheet_, &sprite::msg_player_2, &f_dst);
  }

  /**
   * Preload the sitting pikachu background.
   * Build a new texture stitching / tiling the sprite
   */
  void preload_background() {
    if (sprite_sheet_ == nullptr || renderer_ == nullptr) {
      return;
    }

    // Compute the number of sprite rows and cols with some extra size to allow movement
    constexpr int sprite_width = static_cast<int>(sprite::sitting_pikachu.w);
    constexpr int sprite_height = static_cast<int>(sprite::sitting_pikachu.h);
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

    // Build the sitting pikachu green background
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
      sprite::msg_copyright.w,
      sprite::msg_copyright.h
    ));
    // Set the texture scaling mode to nearest interpolation
    SDL_SetTextureScaleMode(copyright_texture_.get(), SDL_SCALEMODE_NEAREST);

    // Copy the texture from the sheet
    SDL_SetRenderTarget(renderer_, copyright_texture_.get());
    SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::msg_copyright, nullptr);


    // Set the render target back to the main window
    SDL_SetRenderTarget(renderer_, nullptr);
  }
};

} // namespace pika::view


#endif // PIKA_MENU_VIEW_HPP
