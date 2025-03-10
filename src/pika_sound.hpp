#ifndef PIKA_SOUND_HPP
#define PIKA_SOUND_HPP

#include "SDL3/SDL_audio.h"
#include "SDL3_mixer/SDL_mixer.h"

#include <pikaball/common.hpp>
#include <pikaball/physics/physics_common.hpp>  // For FieldSide

namespace pika {

/**
 * Different audio channels that can play sounds at the same time.
 * If the same channel is used to play a new sound, the previous one is stopped.
 */
enum class SoundChannel {
  General = 0,    // Used for menu sounds and start/end sounds
  PikaLeft = 1,   // Used for left player sounds
  PikaRight = 2,  // Used for right player sounds
  Ball = 3        // Used for ball hit/ground sounds
};

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
    // Initialize mixer with support for mp3 files
    Mix_Init(MIX_INIT_MP3);
    // Open default audio device
    audio_dev_id_ = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec_);
    if(audio_dev_id_ == 0) {
      SDL_Log("Unable to open audio! SDL error: %s\n", SDL_GetError());
      throw std::runtime_error("Failed to open audio device");
    }
    // Initialize SDL_mixer
    if(!Mix_OpenAudio(audio_dev_id_, &audio_spec_))
    {
      SDL_Log( "SDL_mixer could not initialize! SDL_mixer error: %s\n", SDL_GetError());
      throw std::runtime_error("SDL_mixer could not initialize");
    }

    // Load sounds
    sound_pi_ = load_chunk(sound_pi_filename);
    sound_pika_ = load_chunk(sound_pika_filename);
    sound_chu_ = load_chunk(sound_chu_filename);
    sound_pikachu_ = load_chunk(sound_pikachu_filename);
    sound_pipikachu_ = load_chunk(sound_pipikachu_filename);
    sound_ball_hit_ = load_chunk(sound_ball_hit_filename);
    sound_ball_ground_ = load_chunk(sound_ball_ground_filename);

    // Allocate channels
    if (Mix_AllocateChannels(num_channels) != num_channels) {
      SDL_Log("Unable to allocate channels! SDL_mixer error: %s\n", SDL_GetError());
      return;
    }
  }

  ~PikaSound() {
    // Free audio chunks
    Mix_FreeChunk(sound_pi_);
    Mix_FreeChunk(sound_pika_);
    Mix_FreeChunk(sound_chu_);
    Mix_FreeChunk(sound_pikachu_);
    Mix_FreeChunk(sound_pipikachu_);
    Mix_FreeChunk(sound_ball_hit_);
    Mix_FreeChunk(sound_ball_ground_);

    // Close audio mixer and audio device
    Mix_CloseAudio();
    Mix_Quit();
    SDL_CloseAudioDevice(audio_dev_id_);
  }

  /** Play the "Pi" sound used in the main menu */
  void pi() const {
    SDL_Log("PI");
    Mix_PlayChannel(static_cast<int>(SoundChannel::General), sound_pi_, 0);
  }

  /** Play the "Pika!" sound (when a player power-hits)
   * @param side The side of the player that triggered the sound.
   *             Used to select the appropriate sound channel.
   */
  void pika(const FieldSide& side) const {
    switch(side) {
    case FieldSide::Left:
      Mix_PlayChannel(static_cast<int>(SoundChannel::PikaLeft), sound_pika_, 0);
      break;
    case FieldSide::Right:
      Mix_PlayChannel(static_cast<int>(SoundChannel::PikaRight), sound_pika_, 0);
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
      Mix_PlayChannel(static_cast<int>(SoundChannel::PikaLeft), sound_pika_, 0);
      break;
    case FieldSide::Right:
      Mix_PlayChannel(static_cast<int>(SoundChannel::PikaRight), sound_pika_, 0);
      break;
    }
  }

  /** Play the "Pikachu" sound (when a game starts) */
  void pikachu() const {
    Mix_PlayChannel(static_cast<int>(SoundChannel::General), sound_pikachu_, 0);
  }

  /** Play the "PiPikachu" sound (when a game ends) */
  void pipikachu() const {
    Mix_PlayChannel(static_cast<int>(SoundChannel::General), sound_pipikachu_, 0);
  }

  /** Play the "Ball hit" sound (when a player power-hits the ball) */
  void ball_hit() const {
    Mix_PlayChannel(static_cast<int>(SoundChannel::Ball), sound_ball_hit_, 0);
  }

  /** Play the sound when the ball touches the ground */
  void ball_ground() const {
    Mix_PlayChannel(static_cast<int>(SoundChannel::Ball), sound_ball_ground_, 0);
  }

private:
  static constexpr SDL_AudioSpec audio_spec_ {
    .format = SDL_AUDIO_F32,
    .channels = 2,
    .freq = 44100,
  };
  SDL_AudioDeviceID audio_dev_id_ {0};

  // Sound chunks
  Mix_Chunk* sound_pi_ {nullptr};
  Mix_Chunk* sound_pika_ {nullptr};
  Mix_Chunk* sound_chu_ {nullptr};
  Mix_Chunk* sound_pikachu_ {nullptr};
  Mix_Chunk* sound_pipikachu_ {nullptr};
  Mix_Chunk* sound_ball_hit_ {nullptr};
  Mix_Chunk* sound_ball_ground_ {nullptr};

  static Mix_Chunk* load_chunk(const char* filename) {
    Mix_Chunk* chunk = Mix_LoadWAV(filename);
    if (chunk == nullptr) {
      SDL_Log("Unable to load sound! Filename: %s - SDL_mixer error: %s\n", filename, SDL_GetError());
      throw std::runtime_error("Failed to open audio file");
    }
    return chunk;
  }
};

} // namespace pika

#endif // PIKA_SOUND_HPP
