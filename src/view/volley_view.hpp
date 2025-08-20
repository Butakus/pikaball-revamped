#ifndef PIKA_VOLLEY_VIEW_HPP
#define PIKA_VOLLEY_VIEW_HPP

#include "view.hpp"
#include "cloud.hpp"
#include "wave.hpp"
#include "ball_view.hpp"
#include "player_view.hpp"
#include "pikaball/input.hpp"
#include "pikaball/physics/physics.hpp"

namespace pika::view {

class VolleyView final : public View {
public:
  // Number of frames for the NewGame state
  constexpr static unsigned int new_game_frames = 71;
  constexpr static unsigned int start_round_frames = 30;
  constexpr static unsigned int end_round_frames = 11;
  constexpr static unsigned int game_end_frames = 211;
  constexpr static unsigned int game_end_skip_frames = 70;

  ~VolleyView() override = default;
  VolleyView(VolleyView const&) = delete;
  VolleyView(VolleyView &&) = delete;
  VolleyView &operator=(VolleyView const&) = delete;
  VolleyView &operator=(VolleyView &&) = delete;

  explicit VolleyView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet) :
    View(renderer, sprite_sheet),
    ball_view_(renderer, sprite_sheet),
    player_view_left_(renderer, sprite_sheet),
    player_view_right_(renderer, sprite_sheet)
  {}


  /**
   * Start the view and reset the game state to start the first round.
   */
  void start() override {
    black_fade_alpha_ = 1.0f;
    if (!background_texture_) {
      preload_background();
    }

    score_left_ = 0;
    score_right_ = 0;
    volley_game_state_ = VolleyGameState::NewGame;
  }

  /**
   * Render the new frame based on the game state and the frame counter.
   * @param frame_counter The current frame counter (used for animations)
   * @param physics_view A const view of the Physics' objects
   */
  void render(const unsigned int frame_counter, const PhysicsView& physics_view) {
    if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
      return;
    }

    // Always render the background, clouds and waves
    render_background();
    // Same with the ball and players
    render_physics(physics_view);
    // And same with the scoreboard
    render_score();

    // Render the remainder objects based on the state
    switch (volley_game_state_) {
      case VolleyGameState::NewGame:
        render_game_start(frame_counter);
      break;
      case VolleyGameState::StartRound:
        // Apply fade in effect
        fade_in(1.0f / 16);
        // Display blinking "Ready" message
        render_ready_msg(frame_counter);
      break;
      case VolleyGameState::PlayRound:
        // If end round -> black_fade_alpha_ = 0;
      break;
      case VolleyGameState::EndRound:
        if (frame_counter >= 6) {
          fade_out(1.0f / 16);
        }
        if (frame_counter >= end_round_frames) {
          // Start the next round
          black_fade_alpha_ = 1.0f;
        }
      break;
      case VolleyGameState::GameEnd:
        // Draw the "Game end" message
        render_game_end(frame_counter);
      break;
    }
  }

  /**
   * Precompute the static background texture from the sprite sheet.
   * If this is not called at initialization, it will be computed at first start()
   */
  void preload_background() {
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

  /**
   * Change the volley game state to know what to render
   * @param state The new VolleyGame state
   */
  void set_state(const VolleyGameState state) {
    volley_game_state_ = state;
  }

  /**
   * Update the players' score
   * @param left Score for left player
   * @param right Score for right player
   */
  void set_score(const unsigned int left, const unsigned int right) {
    score_left_ = left;
    score_right_ = right;
  }

private:
  VolleyGameState volley_game_state_ {VolleyGameState::NewGame};

  unsigned int score_left_ {0};
  unsigned int score_right_ {0};

  // View objects
  SDL_Texture_ptr background_texture_ {nullptr, SDL_DestroyTexture};
  Wave wave_;
  CloudSet clouds_;
  BallView ball_view_;
  PlayerView player_view_left_;
  PlayerView player_view_right_;

  /** Render the whole background */
  void render_background() {
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

  /** Update and render the waves */
  void render_waves() {
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

  /** Update and render the clouds */
  void render_clouds() {
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

  /** Render the ball and players */
  void render_physics(const PhysicsView& physics_view) const {
    // First draw the shadows so they don't get on top of the players
    ball_view_.draw_shadow(physics_view.ball);
    player_view_left_.draw_shadow(physics_view.player_left);
    player_view_right_.draw_shadow(physics_view.player_right);
    // Render ball and players
    player_view_left_.draw_player(physics_view.player_left);
    player_view_right_.draw_player(physics_view.player_right);
    ball_view_.draw_ball(physics_view.ball);
  }

  /** Render the game start message in the NewGame state */
  void render_game_start(const unsigned int frame_counter) {
    if (sprite_sheet_ == nullptr) {
      return;
    }

    // Apply a fade-in the first 17 frames
    fade_in(1.0f / 17);

    // Estimate the message size and position for the current frame_counter
    static constexpr int w = static_cast<int>(sprite::msg_game_start.w);
    static constexpr int h = static_cast<int>(sprite::msg_game_start.h);
    const int half_width = static_cast<int>(w * frame_counter / 50);
    const int half_height = static_cast<int>(h * frame_counter / 50);
    const SDL_FRect dst = {
      .x = static_cast<float>(216 - half_width),
      .y = static_cast<float>(50 + 2 * half_height),
      .w = static_cast<float>(2 * half_width),
      .h = static_cast<float>(2 * half_height),
    };
    // Draw the "game start" message
    SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::msg_game_start, &dst);
  }

  /**
   * Render the "Ready" message when starting a new round
   * The message is toggled every 5 frames
   */
  void render_ready_msg(const unsigned int frame_counter) const {
    // Only show the message every 5 frames (5 yes / 5 no)
    if (!sprite_sheet_ || frame_counter / 5 % 2 == 0) {
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

  /** Render the game start message in the NewGame state */
  void render_game_end(const unsigned int frame_counter) const {
    if (sprite_sheet_ == nullptr) {
      return;
    }

    SDL_FRect dst = {
      .x = 216 - sprite::msg_game_end.w / 2,
      .y = 50,
      .w = sprite::msg_game_end.w,
      .h = sprite::msg_game_end.h,
    };
    if (frame_counter < 50) {
      // In the first frames, reduce the message size
      // Estimate the message size and position for the current frame_counter
      static constexpr int w = static_cast<int>(sprite::msg_game_end.w);
      static constexpr int h = static_cast<int>(sprite::msg_game_end.h);
      const int width_increment = 2 * static_cast<int>(w * (50 - frame_counter) / 50);
      const int height_increment = 2 * static_cast<int>(h * (50 - frame_counter) / 50);
      dst.x -= static_cast<float>(width_increment);
      dst.y -= static_cast<float>(height_increment);
      dst.w += static_cast<float>(2 * width_increment);
      dst.h += static_cast<float>(2 * height_increment);
    }
    // Draw the "game start" message
    SDL_RenderTexture(renderer_, sprite_sheet_, &sprite::msg_game_end, &dst);
  }

  /** Render the players score */
  void render_score() const {
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

};

} // namespace pika::view

#endif // PIKA_VOLLEY_VIEW_HPP
