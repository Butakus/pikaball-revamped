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

private:
  // Non-owning pointer to physics object to update the state of ball and players
  Physics* physics_;

  PlayerInput input_left_ {};
  PlayerInput input_right_ {};

  // View objects
  SDL_Texture_ptr background_texture_ {nullptr, SDL_DestroyTexture};
  Wave wave_;
  CloudSet clouds_;
  BallView ball_view_;
  PlayerView player_view_left_;
  PlayerView player_view_right_;

  /** Render the whole View */
  void render();

  /** Update and render the waves */
  void render_waves();
  /** Update and render the clouds */
  void render_clouds();
};

} // namespace pika::view

#endif // PIKA_VOLLEY_VIEW_HPP
