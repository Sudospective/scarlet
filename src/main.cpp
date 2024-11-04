#include <filesystem>

#include "gizmos.hpp"
#include "scarlet.hpp"

using namespace Scarlet;

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
  luaAnimSprite["Update"] = &AnimatedSprite::Update;
  luaAnimSprite["Draw"] = &AnimatedSprite::Draw;
  luaAnimSprite["LoadTexture"] = &AnimatedSprite::LoadTexture;
  luaAnimSprite["LoadFrameData"] = &AnimatedSprite::LoadFrameData;
  luaAnimSprite["SetCurrentAnimation"] = &AnimatedSprite::SetCurrentAnimation;
}

int main() {
  Engine engine;

  sol::state* lua = Lua::GetInstance().GetState();

  registerGizmos(lua);

  std::string title = "";
  int width = 0;
  int height = 0;
  bool fullscreen = false;

  sol::table config = lua->create_table();

  if (std::filesystem::exists("config.lua")) {
    config = lua->script_file("config.lua");
  }
  if (config["title"] == nullptr) config["title"] = "Scarlet";
  if (config["width"] == nullptr) config["width"] = 640;
  if (config["height"] == nullptr) config["height"] = 360;
  if (config["fullscreen"] == nullptr) config["fullscreen"] = false;

  title = config["title"];
  width = config["width"];
  height = config["height"];
  fullscreen = config["fullscreen"];

  if (engine.Init(title.c_str(), width, height, fullscreen)) {
    engine.Start();
  }

  while (engine.IsRunning()) {
    engine.Update();
    engine.Draw();
  }

  return 0;
}
