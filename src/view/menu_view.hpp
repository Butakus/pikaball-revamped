#ifndef PIKA_MENU_VIEW_HPP
#define PIKA_MENU_VIEW_HPP

#include "view.hpp"
#include "pikaball/input.hpp"


namespace pika::view {

enum class MenuPlayerSelection {
  SINGLE_PLAYER,
  MULTI_PLAYER,
};

class MenuView final : public View {
public:
  // Number of frames at the start to block the input
  constexpr static unsigned int start_frames = 71;

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
  MenuInput input_ {};
  MenuPlayerSelection selection_ {MenuPlayerSelection::SINGLE_PLAYER};
};

} // namespace pika::view


#endif // PIKA_MENU_VIEW_HPP
