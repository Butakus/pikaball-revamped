#ifndef PIKA_INTRO_VIEW_HPP
#define PIKA_INTRO_VIEW_HPP

#include "pikaball/common.hpp"
#include "pikaball/input.hpp"
#include "pikaball/sprites.hpp"
#include "view.hpp"

namespace pika::view {

class IntroView final : public View {

public:
// Total number of frames to render the intro
constexpr static unsigned int max_frames = 165;

private:
// Fixed sprite positions
constexpr static SDL_FRect sachi_dst {
  .x = screen_h_width - sprite::msg_sachisoft.w - 15,
  .y = screen_h_height - sprite::msg_sachisoft.h / 2,
  .w = sprite::msg_sachisoft.w,
  .h = sprite::msg_sachisoft.h,
};
constexpr static SDL_FRect mlp_dst {
  .x = screen_h_width + 15,
  .y = screen_h_height - sprite::msg_init_mark_mlp.h / 2,
  .w = sprite::msg_init_mark_mlp.w,
  .h = sprite::msg_init_mark_mlp.h,
};

public:
  explicit IntroView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet);
  ~IntroView() override = default;

  /**
   * Update the state and render the new frame.s
   * @return The next game state.
   */
  GameState update() override;

  /**
   * Start the view. Set the frame counter to zero.
   */
  void start() override;

  /**
   * Update the menu input to be used in the update() call.
   * Only the enter field is used (to skip the intro).
   * @param input MenuInput
   */
  void set_input(const MenuInput& input);

private:
  MenuInput input_ {};
};

} // namespace pika::view

#endif // PIKA_INTRO_VIEW_HPP
