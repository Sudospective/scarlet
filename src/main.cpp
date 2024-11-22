#include "gizmos.hpp"
#include "scarlet.hpp"

using namespace Scarlet;

SDL_mutex* mutex = nullptr;
Engine* engine = nullptr;

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
  std::cout << "----------------------" << std::endl;
  std::cout << "--- SCARLET ENGINE ---" << std::endl;
  std::cout << "----------------------" << std::endl;

  prefix = "";
  if (argc > 1) {
    prefix = std::string(argv[1]);
  }

  if (!File::Init()) return 1;

  engine = new Engine;

  sol::state* lua = Lua::GetInstance().GetState();

  registerGizmos(lua);

  Scarlet::Log::Print("Loading config...");

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
  SDL_DestroyMutex(mutex);

  delete engine;

  return 0;
}
