#ifndef GIZMO_SOUND_HPP
#define GIZMO_SOUND_HPP

#include <SDL2/SDL_mixer.h>

#include "scarlet.hpp"

#include "gizmos/gizmo.hpp"

class Sound : public Gizmo {
 public:
  Sound() { chunk = nullptr; }
  ~Sound() {}

 public:
  void LoadSource(const char* path) {
    chunk = Mix_LoadWAV(path);
    if (!chunk) {
      Scarlet::Log::Error("Unable to load file '" + std::string(path) + "': " + std::string(Mix_GetError()));
    }
  }
  void Play() {
    if (!chunk) {
      Scarlet::Log::Error("Cannot play sound: no source loaded.");
    }
    Mix_PlayChannel(-1, chunk, 0);
  }
  void SetVolume(float vol) {
    volume = vol;
    float logVol = log(1 + volume);
    Mix_VolumeChunk(chunk, MIX_MAX_VOLUME * logVol);
  }
  float GetVolume() const {
    return volume;
  }

 private:
  float volume;
  Mix_Chunk* chunk;
};

#endif // GIZMO_SOUND_HPP
