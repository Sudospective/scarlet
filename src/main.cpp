#include <filesystem>

#include <SDL2/SDL_thread.h>

#include "gizmos.hpp"
#include "scarlet.hpp"

using namespace Scarlet;

SDL_mutex* mutex = nullptr;
Engine* engine = nullptr;

void registerGizmos(sol::state* lua) {
  sol::usertype<Gizmo> luaGizmo = lua->new_usertype<Gizmo>("Gizmo");
  luaGizmo["name"] = &Gizmo::name;
  luaGizmo["aux"] = &Gizmo::aux;
  luaGizmo["x"] = &Gizmo::x;
  luaGizmo["y"] = &Gizmo::y;
  luaGizmo["Draw"] = &Gizmo::Draw;

  sol::usertype<Quad> luaQuad = lua->new_usertype<Quad>("Quad");
  luaQuad["name"] = &Quad::name;
  luaQuad["aux"] = &Quad::aux;
  luaQuad["color"] = &Quad::color;
  luaQuad["x"] = &Quad::x;
  luaQuad["y"] = &Quad::y;
  luaQuad["w"] = &Quad::w;
  luaQuad["h"] = &Quad::h;
  luaQuad["rot"] = &Quad::rot;
  luaQuad["Draw"] = &Quad::Draw;

  sol::usertype<Sprite> luaSprite = lua->new_usertype<Sprite>("Sprite");
  luaSprite["name"] = &Sprite::name;
  luaSprite["aux"] = &Sprite::aux;
  luaSprite["color"] = &Sprite::color;
  luaSprite["x"] = &Sprite::x;
  luaSprite["y"] = &Sprite::y;
  luaSprite["w"] = &Sprite::w;
  luaSprite["h"] = &Sprite::h;
  luaSprite["rot"] = &Sprite::rot;
  luaSprite["Draw"] = &Sprite::Draw;
  luaSprite["LoadTexture"] = &Sprite::LoadTexture;

  sol::usertype<Label> luaLabel = lua->new_usertype<Label>("Label");
  luaLabel["name"] = &Label::name;
  luaLabel["aux"] = &Label::aux;
  luaLabel["color"] = &Label::color;
  luaLabel["x"] = &Label::x;
  luaLabel["y"] = &Label::y;
  luaLabel["rot"] = &Label::rot;
  luaLabel["align"] = &Label::align;
  luaLabel["Draw"] = &Label::Draw;
  luaLabel["LoadFont"] = &Label::LoadFont;
  luaLabel["text"] = sol::property(&Label::GetText, &Label::SetText);

  sol::usertype<Sound> luaSound = lua->new_usertype<Sound>("Sound");
  luaSound["name"] = &Sound::name;
  luaSound["aux"] = &Sound::aux;
  luaSound["x"] = &Sound::x;
  luaSound["y"] = &Sound::y;
  luaSound["LoadSource"] = &Sound::LoadSource;
  luaSound["Play"] = &Sound::Play;
  luaSound["SetPosition"] = &Sound::SetPosition;
  luaSound["volume"] = sol::property(&Sound::GetVolume, &Sound::SetVolume);

  sol::usertype<AnimatedSprite> luaAnimSprite = lua->new_usertype<AnimatedSprite>("AnimatedSprite");
  luaAnimSprite["name"] = &AnimatedSprite::name;
  luaAnimSprite["aux"] = &AnimatedSprite::aux;
  luaAnimSprite["color"] = &AnimatedSprite::color;
  luaAnimSprite["x"] = &AnimatedSprite::x;
  luaAnimSprite["y"] = &AnimatedSprite::y;
  luaAnimSprite["w"] = sol::property(&AnimatedSprite::GetWidth);
  luaAnimSprite["h"] = sol::property(&AnimatedSprite::GetHeight);
  luaAnimSprite["rot"] = &AnimatedSprite::rot;
  luaAnimSprite["UpdateAnimation"] = &AnimatedSprite::UpdateAnimation;
  luaAnimSprite["Draw"] = &AnimatedSprite::Draw;
  luaAnimSprite["LoadTexture"] = &AnimatedSprite::LoadTexture;
  luaAnimSprite["LoadFrameData"] = &AnimatedSprite::LoadFrameData;
  luaAnimSprite["SetCurrentAnimation"] = &AnimatedSprite::SetCurrentAnimation;
}

int handleUpdate(void*) {
  while (true) {
    SDL_LockMutex(mutex);
    if (!engine->IsRunning()) {
      SDL_UnlockMutex(mutex);
      break;
    }
    engine->Update();
    engine->Draw();
    SDL_UnlockMutex(mutex);
    SDL_Delay(1);
  }
  return 0;
}

int main(int argc, char* argv[]) {
  prefix = "";
  if (argc > 1) {
    prefix = std::string(argv[1]);
  }

  if (!File::Init(argv[0])) return 1;

  engine = new Engine;

  sol::state* lua = Lua::GetInstance().GetState();

  registerGizmos(lua);

  std::string title = "";
  int width = 0;
  int height = 0;
  bool fullscreen = false;
  bool vsync = true;

  sol::table config = lua->create_table();

  std::string code = File::Read("config.lua");
  if (!code.empty()) config = lua->script(code);

  if (config["title"] == nullptr) config["title"] = "Scarlet";
  if (config["width"] == nullptr) config["width"] = 640;
  if (config["height"] == nullptr) config["height"] = 360;
  if (config["fullscreen"] == nullptr) config["fullscreen"] = false;
  if (config["vsync"] == nullptr) config["vsync"] = true;

  title = config["title"];
  width = config["width"];
  height = config["height"];
  fullscreen = config["fullscreen"];
  vsync = config["vsync"];
  
  if (engine->Init(
    title.c_str(),
    width, height,
    fullscreen, vsync
  )) {
    engine->Start();
  }

  mutex = SDL_CreateMutex();

  SDL_Thread* updateThread = SDL_CreateThread(handleUpdate, "UpdateThread", nullptr);

  while (true) {
    SDL_LockMutex(mutex);
    if (!engine->IsRunning()) {
      SDL_UnlockMutex(mutex);
      break;
    }
    engine->HandleEvents();
    SDL_UnlockMutex(mutex);
  }

  SDL_WaitThread(updateThread, nullptr);

  delete engine;

  return 0;
}
