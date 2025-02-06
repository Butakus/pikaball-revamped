#include "menu_view.hpp"


namespace pika::view {

MenuView::MenuView(SDL_Texture *sprite_sheet) : View(sprite_sheet) {}

void MenuView::render(SDL_Renderer* renderer) {
  if (renderer == nullptr || sprite_sheet_ == nullptr) {
    return;
  }

  // Fill the background white
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer);

  frame_counter_++;
  // Check if we must stop the state
  is_finished_ = (frame_counter_ > 71 && input_.enter);
}

void MenuView::set_input(const MenuInput& input) {
  input_ = input;
}

void MenuView::start() {}



} // namespace pika::view