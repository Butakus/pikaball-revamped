#ifndef PIKA_GAME_STATE_HPP
#define PIKA_GAME_STATE_HPP

namespace pika {

// TODO: Combine states inside each View (with its own FSM)
enum class GameState {
  Intro,
  Menu,
  VolleyGame,
};

// Game states in the JS game version (including transitions)
// enum class GameState {
//   Intro,
//   Menu,
//   AfterMenuSelection,
//   BeforeNewGame,
//   StartNewGame,
//   Round,
//   AfterRoundEnd,
//   BeforeNextRound
// };

} // namespace pika

#endif // PIKA_GAME_STATE_HPP
