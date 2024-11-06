#ifndef GIZMO_LABEL_HPP
#define GIZMO_LABEL_HPP

#include <SDL2/SDL_ttf.h>

#include "scarlet.hpp"

#include "gizmos/gizmo.hpp"

class Label : public Gizmo {
 public:
  Label() : Gizmo() {
    broken = false;

    font = nullptr;
    surface = nullptr;
    texture = nullptr;

    if (TTF_Init() < 0) {
      Scarlet::Log::Error("Failed to initialize TTF: " + std::string(SDL_GetError()));
      broken = true;
    }

    sol::state* lua = Scarlet::Lua::GetInstance().GetState();
    color = lua->create_table_with(
      "r", 255u,
      "g", 255u,
      "b", 255u,
      "a", 255u
    );
    align = lua->create_table_with(
      "h", 0.5f,
      "v", 0.5f
    );
  }
  ~Label() {
    TTF_CloseFont(font);
    TTF_Quit();
  }
  void LoadFont(const char* path, int size) {
    if (broken) return;
    if (font)
      TTF_CloseFont(font);
    std::string filepath = Scarlet::prefix + std::string(path);
    font = TTF_OpenFont(filepath.c_str(), size);
  }
  void SetText(const char* newText) {
    text = newText;
    if (broken) return;
    if (font) {
      SDL_Renderer* renderer = Scarlet::Graphics::GetMainRenderer();
      SDL_Color c = {255u, 255u, 255u, 255u};
      surface = TTF_RenderText_Solid(font, text.c_str(), c);
      texture = SDL_CreateTextureFromSurface(renderer, surface);
      w = surface->w;
      h = surface->h;
      SDL_FreeSurface(surface);
    }
  }
  std::string GetText() const {
    return text;
  }
  void Draw() {
    if (broken || !font || text.empty())
      return;
    SDL_Rect rect;
    float alignH = align["h"];
    float alignV = align["v"];
    rect.x = x - w * alignH;
    rect.y = y - h * alignV;
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

 public:
  std::string text;
  sol::table align;
  sol::table color;

 private:
  bool broken;
  float w, h;
  SDL_Surface* surface;
  SDL_Texture* texture;
  TTF_Font* font;
};

#endif // GIZMO_LABEL_HPP
