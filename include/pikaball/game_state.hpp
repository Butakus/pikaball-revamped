#ifndef PIKA_GAME_STATE_HPP
#define PIKA_GAME_STATE_HPP

namespace pika {

/** Main game (outer) states */
enum class GameState {
  Intro,
  Menu,
  VolleyGame,
};

/** Menu sub-states */
enum class MenuState {
  Menu,
  FadeOut
};

/** Possible game modes (selection at menu state) */
enum class MenuPlayerSelection {
  SINGLE_PLAYER,
  MULTI_PLAYER,
};

/** VolleyGame sub-states */
enum class VolleyGameState {
  NewGame,
  StartRound,
  PlayRound,
  EndRound,
  GameEnd
};

} // namespace pika

#endif // PIKA_GAME_STATE_HPP
