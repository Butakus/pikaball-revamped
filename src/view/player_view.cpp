#include "player_view.hpp"

namespace pika::view {

PlayerView::PlayerView(SDL_Renderer *renderer, SDL_Texture *sprite_sheet) :
  renderer_(renderer),
  sprite_sheet_(sprite_sheet)
{}

void PlayerView::draw_player(const Player &player) const {
  // TODO
}


} // namespace pika::view