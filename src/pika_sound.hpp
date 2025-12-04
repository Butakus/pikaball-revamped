#ifndef PIKA_SOUND_HPP
#define PIKA_SOUND_HPP

#include "SDL3/SDL_audio.h"
#include "SDL3_mixer/SDL_mixer.h"

#include <pikaball/resources.hpp>
#include <pikaball/physics/physics_common.hpp>  // For FieldSide

namespace pika {

/**
 * Different audio channels that can play sounds at the same time.
 * If the same channel is used to play a new sound, the previous one is stopped.
 */
// enum class SoundChannel {
//   General = 0,    // Used for menu sounds and start/end sounds
//   PikaLeft = 1,   // Used for left player sounds
//   PikaRight = 2,  // Used for right player sounds
//   Ball = 3        // Used for ball hit/ground sounds
// };

/**
 * A class to handle SDL resources (Window, Renderer, Audio, etc.)
 * Owns and manages the SDL objects.
 * It is responsible for calling SDL_Init and SDL_Quit.
 */
class PikaSound {
public:
  // Number of sound channels (number of elements in SoundChannel enum)
  static constexpr unsigned int num_channels = 4;

  PikaSound(PikaSound const&) = delete;
  PikaSound(PikaSound &&) = delete;
  PikaSound &operator=(PikaSound const&) = delete;
  PikaSound &operator=(PikaSound &&) = delete;

  PikaSound() {
    // Initialize mixer
    MIX_Init();
    // Open default audio device
    audio_dev_id_ = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec_);
    if(audio_dev_id_ == 0) {
      SDL_Log("Unable to open audio! SDL error: %s\n", SDL_GetError());
      throw std::runtime_error("Failed to open audio device");
    }
    // Initialize SDL_mixer
    mixer_ = MIX_CreateMixerDevice(audio_dev_id_, &audio_spec_);
    if(!mixer_)
    {
      SDL_Log( "SDL_mixer could not initialize! SDL_mixer error: %s\n", SDL_GetError());
      throw std::runtime_error("SDL_mixer could not initialize");
    }

    // Load sounds
    sound_pi_ = load_audio(sound_pi_filename);
    sound_pika_ = load_audio(sound_pika_filename);
    sound_chu_ = load_audio(sound_chu_filename);
    sound_pikachu_ = load_audio(sound_pikachu_filename);
    sound_pipikachu_ = load_audio(sound_pipikachu_filename);
    sound_ball_hit_ = load_audio(sound_ball_hit_filename);
    sound_ball_ground_ = load_audio(sound_ball_ground_filename);
    sound_trex_ = load_audio(sound_trex_filename);
    music_background_ = load_audio(music_background_filename);

    // Initialize tracks
    general_track_ = createTrack();
    pika_left_track_ = createTrack();
    pika_right_track_ = createTrack();
    ball_track_ = createTrack();
    music_track_ = createTrack();

    // Setup music track and volume
    MIX_SetTrackAudio(music_track_, music_background_);
    MIX_SetTrackGain(music_track_, 0.8);

    // TODO: Manage track left/right panning
  }

  ~PikaSound() {
    // Free audio chunks
    MIX_DestroyAudio(sound_pi_);
    MIX_DestroyAudio(sound_pika_);
    MIX_DestroyAudio(sound_chu_);
    MIX_DestroyAudio(sound_pikachu_);
    MIX_DestroyAudio(sound_pipikachu_);
    MIX_DestroyAudio(sound_ball_hit_);
    MIX_DestroyAudio(sound_ball_ground_);

    // Close audio mixer and audio device
    MIX_DestroyMixer(mixer_);
    MIX_Quit();
    SDL_CloseAudioDevice(audio_dev_id_);
  }

  /** Play the "Pi" sound used in the main menu */
  void pi() const {
    MIX_SetTrackAudio(general_track_, sound_pi_);
    MIX_PlayTrack(general_track_, {});
  }

  /** Play the "Pika!" sound (when a player power-hits)
   * @param side The side of the player that triggered the sound.
   *             Used to select the appropriate sound channel.
   */
  void pika(const FieldSide& side) const {
    switch(side) {
    case FieldSide::Left:
      MIX_SetTrackAudio(pika_left_track_, sound_pika_);
      MIX_PlayTrack(pika_left_track_, {});
      break;
    case FieldSide::Right:
      MIX_SetTrackAudio(pika_right_track_, sound_pika_);
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
      MIX_SetTrackAudio(pika_left_track_, sound_chu_);
      MIX_PlayTrack(pika_left_track_, {});
      break;
    case FieldSide::Right:
      MIX_SetTrackAudio(pika_right_track_, sound_chu_);
      MIX_PlayTrack(pika_right_track_, {});
      break;
    }
  }

  /** Play the "Pikachu" sound (when a game starts) */
  void pikachu() const {
    MIX_SetTrackAudio(general_track_, sound_pikachu_);
    MIX_PlayTrack(general_track_, {});
  }

  /** Play the "PiPikachu" sound (when a game ends)
   * TODO: Add FieldSide??
   */
  void pipikachu() const {
    MIX_SetTrackAudio(general_track_, sound_pipikachu_);
    MIX_PlayTrack(general_track_, {});
  }

  /** Play the "Ball hit" sound (when a player power-hits the ball) */
  void ball_hit() const {
    MIX_SetTrackAudio(ball_track_, sound_ball_hit_);
    MIX_PlayTrack(ball_track_, {});
  }

  /** Play the sound when the ball touches the ground */
  void ball_ground() const {
    MIX_SetTrackAudio(ball_track_, sound_ball_ground_);
    MIX_PlayTrack(ball_track_, {});
  }

  /** Play the special trex sound */
  void trex() const {
    MIX_SetTrackAudio(general_track_, sound_trex_);
    MIX_PlayTrack(general_track_, {});
  }
  void start_music() const {
    const SDL_PropertiesID play_properties = SDL_CreateProperties();
    SDL_SetNumberProperty(play_properties, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    MIX_PlayTrack(music_track_, play_properties);
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
  static constexpr SDL_AudioSpec audio_spec_ {
    .format = SDL_AUDIO_F32,
    .channels = 2,
    .freq = 44100,
  };
  SDL_AudioDeviceID audio_dev_id_ {0};
  // Mixer and audio tracks
  MIX_Mixer* mixer_;
  MIX_Track* general_track_;
  MIX_Track* pika_left_track_;
  MIX_Track* pika_right_track_;
  MIX_Track* ball_track_;
  MIX_Track* music_track_;

  // Sound chunks
  MIX_Audio* sound_pi_ {nullptr};
  MIX_Audio* sound_pika_ {nullptr};
  MIX_Audio* sound_chu_ {nullptr};
  MIX_Audio* sound_pikachu_ {nullptr};
  MIX_Audio* sound_pipikachu_ {nullptr};
  MIX_Audio* sound_ball_hit_ {nullptr};
  MIX_Audio* sound_ball_ground_ {nullptr};
  MIX_Audio* sound_trex_ {nullptr};
  MIX_Audio* music_background_ {nullptr};

  [[nodiscard]] MIX_Audio* load_audio(const char* filename) const {
    MIX_Audio* chunk = MIX_LoadAudio_IO(
      mixer_,
      load_resource(filename),
      true,
      true
    );
    if (chunk == nullptr) {
      SDL_Log("Unable to load sound! Filename: %s - SDL_mixer error: %s\n", filename, SDL_GetError());
      throw std::runtime_error("Failed to open audio file");
    }
    return chunk;
  }

  [[nodiscard]] MIX_Track* createTrack() const {
    MIX_Track* track = MIX_CreateTrack(mixer_);
    if (track == nullptr) {
      SDL_Log("Unable to create audio track! SDL_mixer error: %s\n", SDL_GetError());
      throw std::runtime_error("Failed to create audio track");
    }
    return track;
  }
};

} // namespace pika

#endif // PIKA_SOUND_HPP
