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

/*
 * Options menu enums.
 * Each one is numbered 0..N-1 in its on-screen order,
 * and declares its number of values in option_count below it.
 */

/** Number of values of an options menu enum (0: not an options menu enum) */
template <typename E>
inline constexpr int option_count = 0;

/** Option menu selection */
enum class OptionMenuSelection {
  Speed = 0,
  Points = 1,
  Music = 2,
};
template <> inline constexpr int option_count<OptionMenuSelection> = 3;

enum class SpeedOptionSelection {
  Slow = 0,
  Medium = 1,
  Fast = 2,
};
template <> inline constexpr int option_count<SpeedOptionSelection> = 3;

enum class PointsOptionSelection {
  Five = 0,
  Ten = 1,
  Fifteen = 2,
};
template <> inline constexpr int option_count<PointsOptionSelection> = 3;

enum class OnOffSelection {
  On = 0,
  Off = 1,
};
template <> inline constexpr int option_count<OnOffSelection> = 2;

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
