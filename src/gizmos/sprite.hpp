#ifndef GIZMO_SPRITE_HPP
#define GIZMO_SPRITE_HPP

//#include <SDL2/SDL_image.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "scarlet.hpp"

#include "gizmos/quad.hpp"

class Sprite : public Quad {
 public:
  Sprite() : Quad() {
    texture = nullptr;
  }
  ~Sprite() {
    if (texture)
      SDL_DestroyTexture(texture);
  }
  void LoadTexture(const char* path) {
    if (texture)
      SDL_DestroyTexture(texture);
    SDL_Renderer* renderer = Scarlet::Graphics::GetMainRenderer();
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    int pitch = width * 4;
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
      static_cast<void*>(data),
      width, height,
      32, pitch,
      SDL_PIXELFORMAT_ABGR8888
    );
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    w = surface->w;
    h = surface->h;
    SDL_FreeSurface(surface);
    stbi_image_free(data);
  }
  void Draw() {
    if (!texture) return;

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

 protected:
  SDL_Texture* texture;
  SDL_Rect rect;
};

#endif // GIZMO_SPRITE_HPP
