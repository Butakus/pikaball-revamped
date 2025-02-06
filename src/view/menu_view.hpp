#ifndef PIKA_MENU_VIEW_HPP
#define PIKA_MENU_VIEW_HPP

#include "view.hpp"
#include "pikaball/input.hpp"


namespace pika::view {

class MenuView final : public View {
public:
  explicit MenuView(SDL_Texture* sprite_sheet);
  ~MenuView() override = default;

  /**
   *  Update the state and render the new frame.
   * @param renderer the Window's renderer to draw objects
   */
  void render(SDL_Renderer* renderer) override;

  /**
   * Set the current menu input. Used to select the number of players and start the game.
   * @param input MenuInput
   */
  void set_input(const MenuInput& input);

  /**
   * Start the menu state. Reset the frame counter
   */
  void start();

  /**
   * @return True if user starts a game
   * TODO: Implement game type selection (1/2 players)
   */
  [[nodiscard]] inline bool is_finished() const { return is_finished_; }

private:
  MenuInput input_ {};
  unsigned int frame_counter_ {0};
  bool is_finished_ {false};
};

} // namespace pika::view


#endif // PIKA_MENU_VIEW_HPP
