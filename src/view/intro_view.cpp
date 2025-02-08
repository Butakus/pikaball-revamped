#include "intro_view.hpp"

namespace pika::view {

IntroView::IntroView(SDL_Renderer* renderer, SDL_Texture* sprite_sheet) :
  View(renderer, sprite_sheet)
{}

GameState IntroView::update() {
  if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
    return GameState::Intro;
  }

  // Fill the background black
  SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer_);

  // Render intro sprites
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::messages_sachisoft, &sachi_dst);
  SDL_RenderTexture(
    renderer_, sprite_sheet_, &sprite::messages_init_mark_mlp, &mlp_dst);

  SDL_RenderPresent(renderer_);

  // Update frame counter and check if the state must change
  frame_counter_++;
  if (frame_counter_ >= max_frames || input_.enter) {
    return GameState::Menu;
  }
  return GameState::Intro;
}

void IntroView::start() {
  frame_counter_ = 0;
  input_ = {};
}

void IntroView::set_input(const MenuInput &input) {
  input_ = input;
}


} // namespace pika::view
