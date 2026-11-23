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
  SinglePlayer,
  MultiPlayer,
};

/** Option menu selection */
enum class OptionMenuSelection {
  Speed = 0,
  Points = 1,
  Music = 2,
};

enum class SpeedOptionSelection {
  Slow = 0,
  Medium = 1,
  Fast = 2,
};

enum class PointsOptionSelection {
  Five = 0,
  Ten = 1,
  Fifteen = 2,
};

enum class OnOffSelection {
  On = 0,
  Off = 1,
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
