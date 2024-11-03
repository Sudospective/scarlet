#ifndef GIZMO_SPRITE_HPP
#define GIZMO_SPRITE_HPP

#include <SDL2/SDL_image.h>

#include "scarlet.hpp"

#include "gizmos/quad.hpp"

class Sprite : public Quad {
 public:
  Sprite() : Quad() {
    texture = nullptr;
  }
  ~Sprite() {
    SDL_DestroyTexture(texture);
  }
  void LoadTexture(const char* path) {
    SDL_Renderer* renderer = Scarlet::Graphics::GetMainRenderer();
    SDL_Surface* surface = IMG_Load(path);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    w = surface->w;
    h = surface->h;
    SDL_FreeSurface(surface);
  }
  void Draw() {
    SDL_Rect rect;
    rect.x = x - w / 2;
    rect.y = y - h / 2;
    rect.w = w;
    rect.h = h;

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
      nullptr,
      &rect,
      rot,
      nullptr,
      SDL_FLIP_NONE
    );
    SDL_SetTextureColorMod(texture, oldColor.r, oldColor.g, oldColor.b);
    SDL_SetTextureAlphaMod(texture, oldColor.a);
    SDL_SetTextureBlendMode(texture, oldBlend);
  }

 private:
  SDL_Texture* texture;
};

#endif // GIZMO_SPRITE_HPP
