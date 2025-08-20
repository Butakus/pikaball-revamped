#ifndef PIKA_GAME_HPP
#define PIKA_GAME_HPP

#include "pikaball/game_state.hpp"
#include "pikaball/input.hpp"
#include "view/intro_view.hpp"
#include "view/menu_view.hpp"
#include "view/volley_view.hpp"
#include "view/options_view.hpp"
#include "sdl_system.hpp"

#include <pikaball/controller/player_controller.hpp>
#include <pikaball/physics/physics.hpp>

namespace pika {

class Game {
public:
  Game();
  ~Game() = default;

  Game(Game const&) = delete;
  Game(Game &&) = delete;
  Game &operator=(Game const&) = delete;
  Game &operator=(Game &&) = delete;

  /**
   * Game loop iteration. Handle input, control game state, and updates views.
   */
  void step();

  /**
   * Run the game continuously at the selected frame rate.
   * This method does not return until the window is closed.
   */
  void run();

private:
  SDLSystem sdl_sys_;

  // Main (and only) physics object to update the state of ball and players
  Physics::Ptr physics_ {nullptr};

  // Views
  std::unique_ptr<view::IntroView> intro_view_ {nullptr};
  std::unique_ptr<view::MenuView> menu_view_ {nullptr};
  std::unique_ptr<view::VolleyView> volley_view_ {nullptr};
  std::unique_ptr<view::OptionsView> options_view_ {nullptr};

  // Frame rate management
  unsigned int target_fps_ {25}; // Possible speeds are 20 / 25 / 30 fps
  // Speed for slow motion effect (fixed to 5 FPS)
  constexpr static unsigned int slow_motion_fps_ {5};
  // Time in nanoseconds per frame. This variable will be changed when slow  motion is applied
  unsigned long target_time_per_frame_ {ns_per_second / target_fps_};

  // Game state
  bool running_ {false};
  bool pause_ {false};
  unsigned int frame_counter_ {0};
  GameState state_ {GameState::Intro};
  // Menu state data
  MenuState menu_state_ {MenuState::Menu};
  MenuPlayerSelection player_selection_ {MenuPlayerSelection::SinglePlayer};
  OptionMenuSelection option_menu_select_ {OptionMenuSelection::Speed};
  SpeedOptionSelection speed_opt_select_ {SpeedOptionSelection::Medium};
  PointsOptionSelection points_opt_select_ {PointsOptionSelection::Fifteen};
  OnOffSelection music_opt_select_ {OnOffSelection::On};

  // VolleyGame state data
  VolleyGameState volley_state_ {VolleyGameState::NewGame};
  unsigned int score_left_ {0};
  unsigned int score_right_ {0};
  unsigned int win_score = 15;

  FieldSide next_serve_side_ {FieldSide::Left};
  // Slow Motion state. The Game object must manually check this to adjust the FPS
  bool slow_motion_ {false};


  // Inputs
  MenuInput menu_input_ {};
  PlayerInput input_left_ {};
  PlayerInput input_right_ {};
  // Player controllers
  std::unique_ptr<PlayerController> controller_left_ {nullptr};
  std::unique_ptr<PlayerController> controller_right_ {nullptr};

  /** Handle keyboard input and update input structs */
  void handle_input();

  /** Handle player and ball sounds */
  void handle_sound() const;
  void handle_player_sound(FieldSide side) const;

  /** Cleans up all the variables after ending a volley game */
  void reset_volley_game_state();
  /** Control the game's logic for the Intro state */
  void intro_state();
  /** Control the game's logic for the main Menu state */
  void menu_state();
  /** Control the game's logic for the Options Menu state */
  void menu_options_state();
  /** Control the game's logic for the VolleyGame state */
  void volley_state();

  /** Update the score based on the position of the ball punch effect.
   *
   * @return The side that won the point
   */
  FieldSide update_score();

  void change_game_speed(SpeedOptionSelection speed);
  void change_win_score(PointsOptionSelection win_points);
  void toggle_music();
};

} // namespace pika

#endif // PIKA_GAME_HPP
