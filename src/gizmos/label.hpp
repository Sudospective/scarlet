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
    texture = nullptr;

    if (TTF_Init() < 0) {
      Scarlet::Log::Error("Failed to initialize TTF: " + std::string(SDL_GetError()));
      broken = true;
    }

    sol::state* lua = Scarlet::Lua::GetInstance().GetState();
    color = lua->create_table_with(
      "r", 1.0f,
      "g", 1.0f,
      "b", 1.0f,
      "a", 1.0f
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
    if (font)
      TTF_CloseFont(font);
    std::vector<uint8_t> data = Scarlet::File::ReadRaw(path);
    if (data.empty()) {
      Scarlet::Log::Error("Unable to load font file: Data empty");
      return;
    }
    SDL_RWops* fontMem = SDL_RWFromConstMem(data.data(), data.size());
    if (!fontMem) {
      Scarlet::Log::Error("Unable to load font into memory: " + std::string(SDL_GetError()));
      return;
    }
    font = TTF_OpenFontRW(fontMem, 1, size);
    if (!font) {
      Scarlet::Log::Error("Unable to open font: " + std::string(TTF_GetError()));
    }
  }
  void SetText(const char* newText) {
    text = newText;
    if (broken || !font) return;
    SDL_Color c = {255u, 255u, 255u, 255u};
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), c);
    if (!surface) {
      Scarlet::Log::Error("Unable to create surface: " + std::string(TTF_GetError()));
      return;
    }

    SDL_Renderer* renderer = Scarlet::Graphics::GetMainRenderer();
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
      Scarlet::Log::Error("Unable to create texture: " + std::string(SDL_GetError()));
      return;
    }

    w = surface->w;
    h = surface->h;
    SDL_FreeSurface(surface);
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
  SDL_Texture* texture;
  TTF_Font* font;
};

#endif // GIZMO_LABEL_HPP
