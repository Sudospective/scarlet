#ifndef GIZMO_ANIMATED_SPRITE_HPP
#define GIZMO_ANIMATED_SPRITE_HPP

#include <vector>

#include "scarlet.hpp"

#include "gizmos/sprite.hpp"

class AnimatedSprite : public Sprite {
 // Declarations
 private:
  struct Frame;
  class Animation;

 public:
  AnimatedSprite() {
    currentAnim = nullptr;
  }
  ~AnimatedSprite() {
    for (Animation* anim : anims) {
      if (anim) delete anim;
    }
  }

 public:
  void LoadFrameData(sol::table luaTable) {
    sol::table luaAnims = luaTable["animations"];
    for (int i = 0; i < luaAnims.size(); i++) {
      sol::table luaAnim = luaAnims[i + 1];
      Animation* anim = new Animation;
      anim->id = i;
      sol::table luaFrames = luaAnim["frames"];
      for (int i = 0; i < luaFrames.size(); i++) {
        sol::table luaFrame = luaFrames[i + 1];
        Frame* frame = new Frame;
        frame->id = i;
        frame->rect = {
          luaFrame["x"], luaFrame["y"],
          luaFrame["w"], luaFrame["h"],
        };
        anim->AddFrame(frame);
      }
      anim->SetSpeed(luaAnim["speed"]);
      AddAnimation(anim);
    }
    SetCurrentAnimation(1);
  }
  void UpdateAnimation(float dt) {
    if (!currentAnim) return;

    Frame* frame = currentAnim->GetCurrentFrame();
    if (!frame) return;

    currentTime += dt;
    if (currentTime >= currentAnim->GetSpeed()) {
      if (frame->id + 1 >= currentAnim->GetNumFrames())
        currentAnim->SetCurrentFrame(0);
      else
        currentAnim->SetCurrentFrame(frame->id + 1);
      currentTime = 0;
    }
  }
  void Draw() {
    if (!texture) return;
    if (!currentAnim) return;

    Frame* frame = currentAnim->GetCurrentFrame();
    if (!frame) return;

    SDL_Rect rect;
    rect.x = x - frame->rect.w / 2;
    rect.y = y - frame->rect.h / 2;
    rect.w = frame->rect.w;
    rect.h = frame->rect.h;

    SDL_Renderer* renderer = Scarlet::Graphics::GetMainRenderer();

    SDL_Color newColor;
    newColor.r = static_cast<Uint8>(255u * static_cast<float>(color["r"]));
    newColor.g = static_cast<Uint8>(255u * static_cast<float>(color["g"]));
    newColor.b = static_cast<Uint8>(255u * static_cast<float>(color["b"]));
    newColor.a = static_cast<Uint8>(255u * static_cast<float>(color["a"]));

    SDL_Color oldColor;
    SDL_BlendMode oldBlend;
    SDL_GetTextureColorMod(texture, &oldColor.r, &oldColor.g, &oldColor.b);
    SDL_GetTextureAlphaMod(texture, &oldColor.a);
    SDL_GetTextureBlendMode(texture, &oldBlend);
    SDL_SetTextureColorMod(texture, newColor.r, newColor.g, newColor.b);
    SDL_SetTextureAlphaMod(texture, newColor.a);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_RenderCopyEx(
      renderer,
      texture,
      &(frame->rect),
      &rect,
      rot,
      nullptr,
      SDL_FLIP_NONE
    );
    SDL_SetTextureColorMod(texture, oldColor.r, oldColor.g, oldColor.b);
    SDL_SetTextureAlphaMod(texture, oldColor.a);
    SDL_SetTextureBlendMode(texture, oldBlend);
  }

 public:
  void SetCurrentAnimation(int index) {
    animIndex = index - 1;
    currentAnim = anims[animIndex];
    currentAnim->SetCurrentFrame(0);
    currentTime = 0;
  }
  Animation* GetCurrentAnimation() const {
    return currentAnim;
  }
  int GetWidth() const {
    return currentAnim->GetCurrentFrame()->rect.w;
  }
  int GetHeight() const {
    return currentAnim->GetCurrentFrame()->rect.h;
  }

 private:
  void AddAnimation(Animation* newAnim) {
    anims.push_back(newAnim);
  }

 private:
  struct Frame {
    int id;
    SDL_Rect rect;
  };
  class Animation {
   public:
    Animation() {
      currentFrame = nullptr;
    }
    ~Animation() {
      for (Frame* frame : frames) {
        if (frame) delete frame;
      }
    }
    void AddFrame(Frame* newFrame) {
      frames.push_back(newFrame);
    }
    void SetSpeed(float spd) {
      speed = spd;
    }
    float GetSpeed() const {
      return speed;
    }
    void SetCurrentFrame(int index) {
      frameIndex = index;
      currentFrame = frames[frameIndex];
    }
    Frame* GetCurrentFrame() const {
      return currentFrame;
    }
    int GetNumFrames() const {
      return frames.size();
    }

   public:
    int id;

   private:
    int frameIndex;
    float speed;
    std::vector<Frame*> frames;
    Frame* currentFrame;
  };

 private:
  int animIndex;
  bool playing;
  float currentTime;
  std::vector<Animation*> anims;
  Animation* currentAnim;
};

#endif // GIZMO_ANIMATED_SPRITE_HPP
