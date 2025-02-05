#ifndef PIKA_VOLLEY_VIEW_HPP
#define PIKA_VOLLEY_VIEW_HPP

#include "view.hpp"
#include "cloud.hpp"
#include "wave.hpp"
#include "pikaball/input.hpp"

namespace pika::view {

class VolleyView final : public View {
public:
  explicit VolleyView(SDL_Texture* sprite_sheet);
  ~VolleyView() override = default;

  /**
   *  Update the state and render the new frame.
   * @param renderer the Window's renderer to draw objects
   */
  void render(SDL_Renderer* renderer) override;

  /**
   * Set the current player input
   * @param input_1 Input for player 1 (left)
   * @param input_2 Input for player 2 (right)
   */
  void set_input(const PlayerInput& input_1, const PlayerInput& input_2);

  /**
   * Precompute the static background texture from the sprite sheet.
   * If this is not called at initialization, it will be computed at first
   * render()
   * @param renderer Window's renderer to generate the new texture.
   */
  void preload_background(SDL_Renderer* renderer);

private:
  PlayerInput input_1_ {};
  PlayerInput input_2_ {};

  // Objects
  SDL_Texture_ptr background_texture_ {nullptr, SDL_DestroyTexture};
  Wave wave_;
  CloudSet clouds_;

  /** Update and render the waves */
  void render_waves(SDL_Renderer* renderer);
  /** Update and render the clouds */
  void render_clouds(SDL_Renderer* renderer);
};

} // namespace pika::view

#endif // PIKA_VOLLEY_VIEW_HPP
