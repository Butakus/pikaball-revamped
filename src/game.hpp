#ifndef PIKA_GAME_HPP
#define PIKA_GAME_HPP

#include <cstdint>
#include <mutex>
#include <optional>

#include "game_state.hpp"
#include "pikaball/input.hpp"
#include "view/intro_view.hpp"
#include "view/menu_view.hpp"
#include "view/volley_view.hpp"
#include "view/options_view.hpp"
#include "view/fps_view.hpp"
#include "sdl_system.hpp"

#include <pikaball/controller/player_controller.hpp>
#include <pikaball/net/connection.hpp>
#include <pikaball/net/network_session.hpp>
#include <pikaball/physics/physics.hpp>

namespace pika {

class Game {
public:
  /**
   * @param net_config Settings to host or join a network game. Empty for a local game.
   */
  explicit Game(const std::optional<net::NetConfig>& net_config = std::nullopt);
  ~Game();

  Game(Game const&) = delete;
  Game(Game &&) = delete;
  Game &operator=(Game const&) = delete;
  Game &operator=(Game &&) = delete;

  /**
   * Game loop iteration. Handle input, control game state, and updates views.
   */
  void step();

  /**
   * Handle a single event from the callback queue.
   * Control-related events are not instantly processed,
   * but stored in a queue and handled during the step update.
   * @param event Pointer to SDL_Event
   */
  void handle_event(const SDL_Event * event);

  /**
   * @return The time until the next step, in nanoseconds.
   *         Very short while waiting for the other machine's input
   */
  [[nodiscard]] std::uint64_t get_frame_duration() const {
    return waiting_for_peer_ ? network_retry_duration_ : target_frame_duration_;
  }
private:
  SDLSystem sdl_sys_;

  // Main (and only) physics object to update the state of ball and players
  Physics::Ptr physics_ {nullptr};

  // Views
  std::unique_ptr<view::IntroView> intro_view_ {nullptr};
  std::unique_ptr<view::MenuView> menu_view_ {nullptr};
  std::unique_ptr<view::VolleyView> volley_view_ {nullptr};
  std::unique_ptr<view::OptionsView> options_view_ {nullptr};
  std::unique_ptr<view::FPSView> fps_view_ {nullptr};

  // Frame rate management
  unsigned int target_fps_ {25}; // Possible speeds are 20 / 25 / 30 fps
  // Speed for slow motion effect (fixed to 5 FPS)
  constexpr static unsigned int slow_motion_fps_ {5};
  // Duration of a frame in nanoseconds. This variable will be changed when slow motion is applied
  std::uint64_t target_frame_duration_ {ns_per_second / target_fps_};
  // Timestamp in nanoseconds of the last frame (to compute FPS)
  std::uint64_t last_frame_timestamp_ {0};
  // FPS estimation
  float current_fps_ {0.0};

  // Game state
  bool pause_ {false};
  bool enable_fps_ {false};  // Flag to display FPS
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
  int score_left_ {0};
  int score_right_ {0};
  int win_score_ = 15;

  FieldSide next_serve_side_ {FieldSide::Left};
  // Slow Motion state. The Game object must manually check this to adjust the FPS
  bool slow_motion_ {false};

  // A mutex to block the event handler while compiling / processing events
  std::mutex events_mutex_;
  std::vector<SDL_Event> events_queue_;

  // Inputs
  // Local menu navigation (not synchronized in network games)
  MenuInput menu_input_ {};
  // Input for the game simulation in the current frame
  TickInput tick_input_ {};
  // Player controllers
  std::unique_ptr<PlayerController> controller_left_ {nullptr};
  std::unique_ptr<PlayerController> controller_right_ {nullptr};

  // Connection to the other machine in a network game. Empty in a local game.
  std::optional<net::Connection> connection_;
  // Input exchange of the current network match. Empty outside network matches.
  std::optional<net::NetworkSession> session_;
  // True if the last step couldn't run because the other machine's input hadn't arrived
  bool waiting_for_peer_ {false};
  // Time between retries while waiting for the other machine's input (0.5 ms)
  constexpr static std::uint64_t network_retry_duration_ {ns_per_second / 2000};
  // While waiting, the Input message is re-sent at this pace (in case the last ones were lost)
  constexpr static std::uint64_t network_resend_interval_ {ns_per_second / 400};
  // Time of the last Input message sent, in nanoseconds
  std::uint64_t last_input_sent_ {0};

  // Statistics of the waits for the other machine's input, logged every second
  struct NetworkWaitStats {
    std::uint64_t period_start {0};  // Start of the logging period (ns)
    unsigned int ticks {0};          // Ticks simulated in the period
    unsigned int waits {0};          // Ticks that had to wait for the other machine
    std::uint64_t total_wait {0};    // Sum of the waits (ns)
    std::uint64_t max_wait {0};      // Longest wait (ns)
  };
  NetworkWaitStats wait_stats_ {};
  // When the wait for the current tick started (ns). Empty if it's not waiting.
  std::optional<std::uint64_t> wait_start_;

  /**
   * Process and compiles all events previously handled by handle_event
   * Afterwards, resets the event queue.
   * Local-only input (menu navigation, FPS toggle) is applied directly.
   * @return The simulation input from this machine's keyboard
   */
  TickInput compile_events();

  /**
   * Apply the simulation input of this frame: pause toggle and player inputs.
   * @param tick The input for this frame
   */
  void apply_tick_input(const TickInput& tick);

  /**
   * Send and receive network messages, log the connection changes,
   * and start or end the network match when the other machine connects or leaves.
   */
  void update_network();

  /**
   * In a network match, send the local input and get the input of the tick to simulate.
   * @param keyboard This machine's keyboard input (either key set controls the local player)
   * @return The input of the next tick, or nothing if the other machine's input hasn't arrived yet
   */
  std::optional<TickInput> exchange_network_input(const TickInput& keyboard);

  /**
   * Record that a network tick is simulated now (and how long it waited), and log the statistics every second
   * @param now Current time (ns)
   */
  void record_network_tick(std::uint64_t now);

  /** Start a network match with the match setup of the connection */
  void start_network_match();
  /** Leave the network match (when it ends, or the other machine leaves) */
  void end_network_match();
  /** Render the screen shown while waiting for the other machine */
  void network_waiting_state();

  /** Switch to the VolleyGame state and start the game (from the menu, or a network match) */
  void start_volley_game();

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
  /**
   * Query both controllers and advance the physics one step.
   * @return true if the ball touched the ground (same as Physics::update)
   */
  bool step_physics();
  /** Display the FPS */
  void display_fps();

  /** Update the score based on the position of the ball punch effect.
   *
   * @return The side that won the point
   */
  FieldSide update_score();

  void change_game_speed(SpeedOptionSelection speed);
  void change_win_score(PointsOptionSelection win_points);
  void change_music(OnOffSelection music);
};

} // namespace pika

#endif // PIKA_GAME_HPP
