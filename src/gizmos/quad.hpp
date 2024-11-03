#ifndef GIZMO_QUAD_HPP
#define GIZMO_QUAD_HPP

#include "scarlet.hpp"

#include "gizmos/gizmo.hpp"

class Quad : public Gizmo {
 public:
  Quad() : Gizmo() {
    sol::state* lua = Scarlet::Lua::GetInstance().GetState();
    color = lua->create_table_with(
      "r", 1.0f,
      "g", 1.0f,
      "b", 1.0f,
      "a", 1.0f
    );

    w = h = 0.0f;
  }
  void Draw() {
    SDL_Rect rect;
    rect.x = x - w / 2;
    rect.y = y - h / 2;
    rect.w = w;
    rect.h = h;

    SDL_Renderer* renderer = Scarlet::Graphics::GetMainRenderer();
    SDL_Texture* texture = Scarlet::Graphics::GetDefaultTexture();

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
  };

 public:
  float w, h;
  sol::table color;
};

#endif // GIZMO_QUAD_HPP
