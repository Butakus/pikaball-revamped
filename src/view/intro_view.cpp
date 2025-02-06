#include "intro_view.hpp"

namespace pika::view {

IntroView::IntroView(SDL_Texture* sprite_sheet) : View(sprite_sheet) {}

void IntroView::render(SDL_Renderer* renderer) {
  if (renderer == nullptr || sprite_sheet_ == nullptr) {
    return;
  }

  // Fill the background black
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer);

  // Render intro sprites
  SDL_RenderTexture(
    renderer, sprite_sheet_, &sprite::messages_sachisoft, &sachi_dst);
  SDL_RenderTexture(
    renderer, sprite_sheet_, &sprite::messages_init_mark_mlp, &mlp_dst);
}



} // namespace pika::view
