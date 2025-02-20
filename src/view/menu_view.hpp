#ifndef PIKA_MENU_VIEW_HPP
#define PIKA_MENU_VIEW_HPP

#include "view.hpp"
#include "pikaball/input.hpp"


namespace pika::view {

enum class MenuPlayerSelection {
  SINGLE_PLAYER,
  MULTI_PLAYER,
};

enum class MenuState {
  Menu,
  FadeOut
};

class MenuView final : public View {
public:
  // Number of frames at the start to block the input
  constexpr static unsigned int start_frames = 71;
  // Number of frames for the final fade out effect
  constexpr static unsigned int fade_out_frames = 30;

  explicit MenuView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet);
  ~MenuView() override = default;

  /**
   * Update the state and render the new frame.
   * @return The next game state.
   */
  GameState update() override;

  /**
   * Update the menu input to be used in the update() call.
   * @param input MenuInput
   */
  void set_input(const MenuInput& input);

  /**
   * Start the view
   * Set the frame counter to zero and the player selection to 1P.
   */
  void start() override;

  /**
   * Get the selected game mode (1 / 2 players)
   * @return The MenuPlayerSelection value.
   */
  [[nodiscard]] MenuPlayerSelection get_selection() const { return selection_; }

private:
  MenuState state_ {MenuState::Menu};
  MenuInput input_ {};
  MenuPlayerSelection selection_ {MenuPlayerSelection::SINGLE_PLAYER};
  // Background with the repeating sitting pikachus
  SDL_Texture_ptr background_texture_{nullptr, SDL_DestroyTexture};
  // Copyright message. Needs its own texture to apply an independent alpha.
  SDL_Texture_ptr copyright_texture_{nullptr, SDL_DestroyTexture};
  int background_offset_ {0};
  float pika_background_alpha_ {0.0};
  float copyright_alpha_ {0.0};
  unsigned int selection_size_ {2};

  /** Render the whole View */
  void render();

  /** Render the background: Sitting pikachu moving diagonally. */
  void render_background();

  /** Render the "fight!" / "MLP powah!" sprite */
  void render_fight_msg() const;
  /** Render the copyright message at the bottom */
  void render_copyright_msg();
  /** Render the title sprites (Pikachu Volleyball / Tournament) */
  void render_title_msgs() const;
  /** Render 1P/2P selection messages */
  void render_player_selection_msg();

  /**
   * Preload the sitting pikachu background.
   * Build a new texture stitching / tiling the sprite
   */
  void preload_background();
};

} // namespace pika::view


#endif // PIKA_MENU_VIEW_HPP
