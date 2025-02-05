#ifndef PIKA_INTRO_VIEW_HPP
#define PIKA_INTRO_VIEW_HPP

#include "pikaball/common.hpp"
#include "pikaball/input.hpp"
#include "pikaball/sprites.hpp"
#include "view.hpp"

namespace pika::view {

class IntroView final : public View{

// Total number of frames to render the intro
constexpr static unsigned int max_frames = 165;

// Fixed sprite positions
constexpr static SDL_FRect sachi_dst {
  .x = screen_h_width - sprite::messages_sachisoft.w - 15,
  .y = screen_h_height - sprite::messages_sachisoft.h / 2,
  .w = sprite::messages_sachisoft.w,
  .h = sprite::messages_sachisoft.h,
};
constexpr static SDL_FRect mlp_dst {
  .x = screen_h_width + 15,
  .y = screen_h_height - sprite::messages_init_mark_mlp.h / 2,
  .w = sprite::messages_init_mark_mlp.w,
  .h = sprite::messages_init_mark_mlp.h,
};

public:
  explicit IntroView(SDL_Texture* sprite_sheet);
  ~IntroView() override = default;

  /**
   *  Update the state and render the new frame.
   * @param renderer the Window's renderer to draw objects
   */
  void render(SDL_Renderer* renderer) override;

  /**
   * Set the current menu input. This is used to skip the intro.
   * @param input MenuInput (only the power-hit commands are used)
   */
  void set_input(const MenuInput& input);

  /**
   * Start the intro state. Reset the frame counter
   */
  void start();

  /**
   * @return True if the frame counter reached the end
   */
  [[nodiscard]] inline bool is_finished() const { return is_finished_; }
  
private:
  MenuInput input_ {};
  unsigned int frame_counter_ {0};
  bool is_finished_ {false};

  // Objects
  SDL_Texture_ptr background_texture_ {nullptr, SDL_DestroyTexture};

};

} // namespace pika::view

#endif // PIKA_INTRO_VIEW_HPP
