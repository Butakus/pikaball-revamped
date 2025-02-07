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
}

void MenuView::update(const MenuInput& input) {
  if (selection_ == MenuPlayerSelection::SINGLE_PLAYER && input.down) {
    selection_ = MenuPlayerSelection::MULTI_PLAYER;
  } else if (selection_ == MenuPlayerSelection::MULTI_PLAYER && input.up) {
    selection_ = MenuPlayerSelection::SINGLE_PLAYER;
  }

  // switch (selection_) {
  //   case MenuPlayerSelection::SINGLE_PLAYER:
  //     if (input.down) {
  //       selection_ = MenuPlayerSelection::MULTI_PLAYER;
  //     }
  //     break;
  //   case MenuPlayerSelection::MULTI_PLAYER:
  //     if (input.up) {
  //       selection_ = MenuPlayerSelection::SINGLE_PLAYER;
  //     }
  //     break;
  // }

}

void MenuView::start() {
  selection_ = MenuPlayerSelection::SINGLE_PLAYER;
}

} // namespace pika::view