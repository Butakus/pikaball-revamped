#ifndef PIKA_SOUND_HPP
#define PIKA_SOUND_HPP

#include "SDL3/SDL_audio.h"
#include "SDL3_mixer/SDL_mixer.h"

#include <memory>
#include <string>

#include <pikaball/resources.hpp>
#include <pikaball/sdl_error.hpp>
#include <pikaball/physics/physics_common.hpp>  // For FieldSide

namespace pika {

/**
 * Game audio: owns the SDL_mixer mixer (on the default playback device),
 * the loaded sounds and music, and the tracks that play them.
 * It is responsible for calling MIX_Init and MIX_Quit.
 */
class PikaSound {
public:
  PikaSound(PikaSound const&) = delete;
  PikaSound(PikaSound &&) = delete;
  PikaSound &operator=(PikaSound const&) = delete;
  PikaSound &operator=(PikaSound &&) = delete;

  /**
   * Initialize SDL_mixer, open the default audio device and load all sounds.
   * Throws if any step fails.
   */
  PikaSound() {
    if (!MIX_Init()) {
      throw sdl_error("Failed to init SDL_mixer");
    }
    // The destructor doesn't run if the constructor throws, so clean up manually
    try {
      init();
    } catch (...) {
      shutdown();
      throw;
    }
  }

  ~PikaSound() {
    shutdown();
  }

  /** Play the "Pi" sound used in the main menu */
  void pi() const {
    MIX_SetTrackAudio(general_track_, sound_pi_.get());
    MIX_PlayTrack(general_track_, {});
  }

  /** Play the "Pika!" sound (when a player power-hits)
   * @param side The side of the player that triggered the sound.
   *             Used to select the appropriate sound channel.
   */
  void pika(const FieldSide& side) const {
    switch(side) {
    case FieldSide::Left:
      MIX_SetTrackAudio(pika_left_track_, sound_pika_.get());
      MIX_PlayTrack(pika_left_track_, {});
      break;
    case FieldSide::Right:
      MIX_SetTrackAudio(pika_right_track_, sound_pika_.get());
      MIX_PlayTrack(pika_right_track_, {});
      break;
    }
  }

  /** Play the "Chu" sound (when a player jumps or dives)
   * @param side The side of the player that triggered the sound.
   *             Used to select the appropriate sound channel.
   */
  void chu(const FieldSide& side) const {
    switch(side) {
    case FieldSide::Left:
      MIX_SetTrackAudio(pika_left_track_, sound_chu_.get());
      MIX_PlayTrack(pika_left_track_, {});
      break;
    case FieldSide::Right:
      MIX_SetTrackAudio(pika_right_track_, sound_chu_.get());
      MIX_PlayTrack(pika_right_track_, {});
      break;
    }
  }

  /** Play the "Pikachu" sound (when a game starts) */
  void pikachu() const {
    MIX_SetTrackAudio(general_track_, sound_pikachu_.get());
    MIX_PlayTrack(general_track_, {});
  }

  /** Play the "PiPikachu" sound (when a game ends)
   * TODO: Add FieldSide??
   */
  void pipikachu() const {
    MIX_SetTrackAudio(general_track_, sound_pipikachu_.get());
    MIX_PlayTrack(general_track_, {});
  }

  /** Play the "Ball hit" sound (when a player power-hits the ball) */
  void ball_hit() const {
    MIX_SetTrackAudio(ball_track_, sound_ball_hit_.get());
    MIX_PlayTrack(ball_track_, {});
  }

  /** Play the sound when the ball touches the ground */
  void ball_ground() const {
    MIX_SetTrackAudio(ball_track_, sound_ball_ground_.get());
    MIX_PlayTrack(ball_track_, {});
  }

  void start_music() const {
    const SDL_PropertiesID play_properties = SDL_CreateProperties();
    SDL_SetNumberProperty(play_properties, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    MIX_PlayTrack(music_track_, play_properties);
    SDL_DestroyProperties(play_properties);
  }

  void pause_music() const {
    MIX_PauseTrack(music_track_);
  }
  void resume_music() const {
    MIX_ResumeTrack(music_track_);
  }

  void stop_music() const {
    MIX_StopTrack(music_track_, MIX_TrackMSToFrames(music_track_, 200));
  }

private:
  using MIX_Mixer_ptr = std::unique_ptr<MIX_Mixer, decltype(&MIX_DestroyMixer)>;
  using MIX_Audio_ptr = std::unique_ptr<MIX_Audio, decltype(&MIX_DestroyAudio)>;

  static constexpr SDL_AudioSpec audio_spec_ {
    .format = SDL_AUDIO_F32,
    .channels = 2,
    .freq = 44100,
  };

  MIX_Mixer_ptr mixer_ {nullptr, MIX_DestroyMixer};

  // Sound chunks
  MIX_Audio_ptr sound_pi_ {nullptr, MIX_DestroyAudio};
  MIX_Audio_ptr sound_pika_ {nullptr, MIX_DestroyAudio};
  MIX_Audio_ptr sound_chu_ {nullptr, MIX_DestroyAudio};
  MIX_Audio_ptr sound_pikachu_ {nullptr, MIX_DestroyAudio};
  MIX_Audio_ptr sound_pipikachu_ {nullptr, MIX_DestroyAudio};
  MIX_Audio_ptr sound_ball_hit_ {nullptr, MIX_DestroyAudio};
  MIX_Audio_ptr sound_ball_ground_ {nullptr, MIX_DestroyAudio};
  MIX_Audio_ptr music_background_ {nullptr, MIX_DestroyAudio};

  // Audio tracks. Non-owning: they are owned (and destroyed) by the mixer.
  MIX_Track* general_track_ {nullptr};
  MIX_Track* pika_left_track_ {nullptr};
  MIX_Track* pika_right_track_ {nullptr};
  MIX_Track* ball_track_ {nullptr};
  MIX_Track* music_track_ {nullptr};

  /**
   * Create the mixer, load all sounds and create the tracks (constructor body).
   * Throws on failure.
   */
  void init() {
    // Create the mixer. This opens the default playback device itself.
    mixer_.reset(MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec_));
    if (!mixer_) {
      throw sdl_error("Failed to open audio device");
    }

    // Load sounds
    sound_pi_ = load_audio(sound_pi_filename);
    sound_pika_ = load_audio(sound_pika_filename);
    sound_chu_ = load_audio(sound_chu_filename);
    sound_pikachu_ = load_audio(sound_pikachu_filename);
    sound_pipikachu_ = load_audio(sound_pipikachu_filename);
    sound_ball_hit_ = load_audio(sound_ball_hit_filename);
    sound_ball_ground_ = load_audio(sound_ball_ground_filename);
    music_background_ = load_audio(music_background_filename);

    // Initialize tracks
    general_track_ = create_track();
    pika_left_track_ = create_track();
    pika_right_track_ = create_track();
    ball_track_ = create_track();
    music_track_ = create_track();

    // Setup music track and volume
    MIX_SetTrackAudio(music_track_, music_background_.get());
    MIX_SetTrackGain(music_track_, 0.8);

    // TODO: Manage track left/right panning
  }

  /** Release all resources in reverse creation order, then quit SDL_mixer. */
  void shutdown() noexcept {
    // Tracks are owned by the mixer: MIX_DestroyMixer destroys them
    general_track_ = nullptr;
    pika_left_track_ = nullptr;
    pika_right_track_ = nullptr;
    ball_track_ = nullptr;
    music_track_ = nullptr;

    music_background_.reset();
    sound_ball_ground_.reset();
    sound_ball_hit_.reset();
    sound_pipikachu_.reset();
    sound_pikachu_.reset();
    sound_chu_.reset();
    sound_pika_.reset();
    sound_pi_.reset();
    mixer_.reset();
    MIX_Quit();
  }

  [[nodiscard]] MIX_Audio_ptr load_audio(const char* filename) const {
    MIX_Audio_ptr chunk {
      MIX_LoadAudio_IO(mixer_.get(), load_resource(filename), true, true),
      MIX_DestroyAudio
    };
    if (!chunk) {
      throw sdl_error(std::string("Failed to load sound '") + filename + "'");
    }
    return chunk;
  }

  [[nodiscard]] MIX_Track* create_track() const {
    MIX_Track* track = MIX_CreateTrack(mixer_.get());
    if (track == nullptr) {
      throw sdl_error("Failed to create audio track");
    }
    return track;
  }
};

} // namespace pika

#endif // PIKA_SOUND_HPP
