#ifndef GIZMO_SOUND_HPP
#define GIZMO_SOUND_HPP

#include <SDL2/SDL_mixer.h>

#include "scarlet.hpp"

#include "gizmos/gizmo.hpp"

class Sound : public Gizmo {
 public:
  Sound() {
    chunk = nullptr;
    channel = -1;
  }
  ~Sound() {
    if (chunk)
      Mix_FreeChunk(chunk);
  }

 public:
  void LoadSource(const char* path) {
    std::string filepath = Scarlet::prefix + std::string(path);
    chunk = Mix_LoadWAV(filepath.c_str());
    if (!chunk) {
      Scarlet::Log::Error("Unable to load file '" + filepath + "': " + std::string(Mix_GetError()));
    }
  }
  void Play() {
    if (!chunk) {
      Scarlet::Log::Error("Cannot play sound: no source loaded.");
    }
    channel = Mix_PlayChannel(-1, chunk, 0);
  }
  void SetVolume(float vol) {
    volume = vol;
    float logVol = log(1 + volume);
    Mix_VolumeChunk(chunk, MIX_MAX_VOLUME * logVol);
  }
  float GetVolume() const {
    return volume;
  }
  void SetPosition() {
    sol::state* lua = Scarlet::Lua::GetInstance().GetState();
    float sw = (*lua)["scarlet"]["window"]["width"];
    int left = static_cast<Uint8>(SDL_clamp((sw - x) / sw, 0.0f, 1.0f) * 255u);
    Mix_SetPanning(channel, left, 255 - left);
  }

 private:
  float volume;
  int channel;
  Mix_Chunk* chunk;
};

#endif // GIZMO_SOUND_HPP
