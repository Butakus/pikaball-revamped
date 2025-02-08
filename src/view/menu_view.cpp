#include "menu_view.hpp"


namespace pika::view {

MenuView::MenuView(SDL_Renderer *renderer, SDL_Texture *sprite_sheet) :
  View(renderer, sprite_sheet)
{}

GameState MenuView::update() {
  if (renderer_ == nullptr || sprite_sheet_ == nullptr) {
    return GameState::Menu;
  }

  // Fill the background white or black
  if (selection_ == MenuPlayerSelection::SINGLE_PLAYER) {
    SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
  } else {
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);
  }
  SDL_RenderClear(renderer_);

  // TODO: Render the background and messages

  SDL_RenderPresent(renderer_);

  frame_counter_++;
  if (frame_counter_ < start_frames && input_.enter) {
    // The animation is skipped by pressing enter
    frame_counter_ = start_frames;
    return GameState::Menu;
  }
  // On the first frames, just render the animation (skip input processing).
  if (frame_counter_ <= start_frames) {
    return GameState::Menu;
  }

  // Process input to update the game mode selection
  if (selection_ == MenuPlayerSelection::SINGLE_PLAYER && input_.down) {
    selection_ = MenuPlayerSelection::MULTI_PLAYER;
  } else if (selection_ == MenuPlayerSelection::MULTI_PLAYER && input_.up) {
    selection_ = MenuPlayerSelection::SINGLE_PLAYER;
  }

  // Process input to check if the game must start
  if (input_.enter) {
    return GameState::Round;
  }

  return GameState::Menu;
}

void MenuView::start() {
  frame_counter_ = 0;
  input_ = {};
  selection_ = MenuPlayerSelection::SINGLE_PLAYER;
}

void MenuView::set_input(const MenuInput& input) {
  input_ = input;
}

} // namespace pika::view