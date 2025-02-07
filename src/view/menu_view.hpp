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

  explicit MenuView(SDL_Texture* sprite_sheet);
  ~MenuView() override = default;

  /**
   *  Update the state and render the new frame.
   * @param renderer the Window's renderer to draw objects
   */
  void render(SDL_Renderer* renderer) override;

  /**
   * Update the menu state based on the current menu input.
   * Used to select the number of players and start the game.
   * @param input MenuInput
   */
  void update(const MenuInput& input);

  /**
   * Start the menu state. Reset the player selection
   */
  void start();

  /**
   * Get the selected game mode (1 / 2 players)
   * @return The MenuPlayerSelection value.
   */
  [[nodiscard]] MenuPlayerSelection get_selection() const { return selection_; }

private:
  MenuPlayerSelection selection_ {MenuPlayerSelection::SINGLE_PLAYER};
};

} // namespace pika::view


#endif // PIKA_MENU_VIEW_HPP
