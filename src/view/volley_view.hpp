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

enum class VolleyGameState {
  NewGame,
  StartRound,
  PlayRound,
  EndRound,
  GameEnd
};

class VolleyView final : public View {
public:
  // Number of frames for the NewGame state
  constexpr static unsigned int new_game_frames = 71;
  constexpr static unsigned int start_round_frames = 30;
  constexpr static unsigned int end_round_frames = 11;
  constexpr static unsigned int game_end_frames = 211;
  constexpr static unsigned int game_end_skip_frames = 70;
  // TODO: This must come from the game settings
  constexpr static unsigned int win_score = 5;

  explicit VolleyView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet, Physics* physics);
  ~VolleyView() override = default;

  /**
   * Update the state and render the new frame.
   * @return The next game state.
   */
  GameState update() override;

  /**
   * Start the view and reset the game state to start the first round.
   */
  void start() override;

  /**
   * Set the current player input
   * @param input_left Input for player 1 (left)
   * @param input_right Input for player 2 (right)
   */
  void set_input(const PlayerInput& input_left, const PlayerInput& input_right);

  /**
   * Precompute the static background texture from the sprite sheet.
   * If this is not called at initialization, it will be computed at first update()
   */
  void preload_background();

  /**
   * Check if the view needs the game to slow down the FPS.
   *
   * This is not elegant (the game object must check this everytime),
   * but it is what it is right now.
   * @return True if the game should run in slow motion.
   */
  bool slow_motion() const { return slow_motion_; }

private:
  // Non-owning pointer to physics object to update the state of ball and players
  Physics* physics_;

  PlayerInput input_left_ {};
  PlayerInput input_right_ {};

  VolleyGameState volley_game_state_ {VolleyGameState::NewGame};

  unsigned int score_left_ {0};
  unsigned int score_right_ {0};
  FieldSide next_serve_side_ {FieldSide::Left};

  // Slow Motion state. The Game object must manually check this to adjust the FPS
  bool slow_motion_ {false};

  // View objects
  SDL_Texture_ptr background_texture_ {nullptr, SDL_DestroyTexture};
  Wave wave_;
  CloudSet clouds_;
  BallView ball_view_;
  PlayerView player_view_left_;
  PlayerView player_view_right_;

  /** Render the whole background */
  void render_background();
  /** Update and render the waves */
  void render_waves();
  /** Update and render the clouds */
  void render_clouds();

  /** Render the ball and players */
  void render_physics() const;

  /** Render the game start message in the NewGame state */
  void render_game_start();
  /**
   * Render the "Ready" message when starting a new round
   * The message is toggled every 5 frames
   */
  void render_ready_msg() const;
  /** Render the game start message in the NewGame state */
  void render_game_end() const;
  /** Render the players score */
  void render_score() const;

  /** Update the score based on the position of the ball punch effect.
   *
   * @return The side that won the point
   */
  FieldSide update_score();
};

} // namespace pika::view

#endif // PIKA_VOLLEY_VIEW_HPP
