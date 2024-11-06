#ifndef SCARLET_HPP
#define SCARLET_HPP

#include <iostream>
#include <string>
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <sol/sol.hpp>

// Scarlet Declaration
namespace Scarlet {
  class Log;
  class Lua;
  class Input;
  class Audio;
  class Graphics;
  class Engine;
};

// Scarlet Implementation
namespace Scarlet {
  class Log {
   public:
    static void Print(std::string msg) {
      std::cout << "SCARLET: " << msg << std::endl;
    }
    static void Error(std::string msg) {
      std::cerr << "SCARLET: " << msg << std::endl;
    }
  };

  class Lua {
   public:
    Lua(const Lua&) = delete;
  
   public:
    static Lua& GetInstance() {
      if (!instance)
        instance = new Lua;
      return *instance;
    }
    sol::state* GetState() {
      return state;
    }

   public:
    Lua& operator=(const Lua&) = delete;

   private:
    Lua() {
      state = new sol::state;
      state->open_libraries(
        sol::lib::base,
        sol::lib::coroutine,
        sol::lib::math,
        sol::lib::string,
        sol::lib::package
      );
    }
    ~Lua() {
      delete state;
    }

   private:
    sol::state* state;
    inline static Lua* instance = nullptr;
  };

  class Input {
   public:
    Input(const Input&) = delete;
    static Input& GetInstance() {
      if (!instance) {
        instance = new Input;
      }
      return *instance;
    }

   public:
    void Init() {
      Log::Print("Initializing input...");

      sol::state* lua = Lua::GetInstance().GetState();

      luaController = lua->new_usertype<Controller>("Controller", sol::no_constructor);
      luaController.set("id", sol::readonly(&Controller::id));
      luaController.set("type", sol::readonly(&Controller::type));

      controllers = lua->create_table();

      Log::Print("Input initialized.");
    }
    void FindControllers() {
      Log::Print("Enumerating controllers...");

      Log::Print("Detected " + std::to_string(SDL_NumJoysticks()) + " joysticks...");

      for (int i = 0; i < SDL_NumJoysticks(); i++) {
        SDL_GameController* gamepad;
        if (SDL_IsGameController(i)) {
          gamepad = SDL_GameControllerOpen(i);
        }
        Controller c;
        c.gamepad = gamepad;
        c.id = SDL_JoystickGetDeviceInstanceID(i);
        c.type = SDL_GameControllerGetType(gamepad);
        controllers[i + 1] = c;
      }

      Log::Print("Controllers enumerated.");
      Log::Print(std::to_string(controllers.size()) + " controller(s) connected.");
    }
    void HandleEvent(SDL_Event* event) {
      sol::state* lua = Lua::GetInstance().GetState();
      sol::table luaEvent = lua->create_table();
      switch (event->type) {
        // Keyboard and Mouse
        case SDL_KEYDOWN: {
          if (event->key.repeat > 0)
            luaEvent["type"] = "KeyRepeat";
          else
            luaEvent["type"] = "KeyDown";
          luaEvent["button"] = SDL_GetKeyName(event->key.keysym.sym);
          break;
        }
        case SDL_KEYUP: {
          luaEvent["type"] = "KeyUp";
          luaEvent["button"] = SDL_GetKeyName(event->key.keysym.sym);
          break;
        }
        case SDL_MOUSEBUTTONDOWN: {
          luaEvent["type"] = "MouseDown";
          luaEvent["button"] = event->button.button;
          break;
        }
        case SDL_MOUSEBUTTONUP: {
          luaEvent["type"] = "MouseUp";
          luaEvent["button"] = event->button.button;
          break;
        }
        // Controller
        case SDL_CONTROLLERBUTTONDOWN: {
          luaEvent["type"] = "GamepadDown";
          luaEvent["id"] = event->cdevice.which;
          luaEvent["button"] = event->cbutton.button;
          break;
        }
        case SDL_CONTROLLERBUTTONUP: {
          luaEvent["type"] = "GamepadUp";
          luaEvent["id"] = event->cdevice.which;
          luaEvent["button"] = event->cbutton.button;
          break;
        }
        case SDL_CONTROLLERAXISMOTION: {
          luaEvent["type"] = "GamepadAxis";
          luaEvent["id"] = event->cdevice.which;
          luaEvent["axis"] = event->caxis.axis;
          luaEvent["value"] = event->caxis.value;
          break;
        }
      }
      (*lua)["input"](luaEvent);
    }

   public:
    Input& operator=(const Input&) = delete;

   private:
    Input() {}
    ~Input() {
      for (int i = 0; i < controllers.size(); i++) {
        Controller& c = controllers[i + 1];
        SDL_GameControllerClose(c.gamepad);
      }
    }

   private:
    struct Controller {
      std::string type;
      Sint32 id;
      SDL_GameController* gamepad;
    };
    sol::usertype<Controller> luaController;

   private:
    sol::table controllers;
    inline static Input* instance = nullptr;
  };

  class Audio {
   public:
    Audio(const Audio&) = delete;
    static Audio& GetInstance() {
      if (!instance) {
        instance = new Audio;
      }
      return *instance;
    }

   public:
    void Init() {
      Log::Print("Initializing Audio...");
      Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
      Mix_OpenAudio(
        MIX_DEFAULT_FREQUENCY,
        MIX_DEFAULT_FORMAT,
        2, 1024
      );

      sol::state* lua = Lua::GetInstance().GetState();

      std::function getVol = [&]() { return this->GetVolume(); };
      std::function setVol = [&](float vol) { this->SetVolume(vol); };

      sol::table musicTable = lua->create_table_with(
        "play", [&](const char* path) { this->PlayMusic(path); },
        "stop", [&]() { if (this->music) this->StopMusic(); },
        "volume", sol::property(getVol, setVol)
      );
      (*lua)["scarlet"]["music"] = musicTable;

      Log::Print("Audio initialized.");
    }
    void PlayMusic(const char* path) {
      StopMusic();
      music = Mix_LoadMUS(path);
      if (!music) {
        Log::Error("Unable to load file '" + std::string(path) + "': " + std::string(Mix_GetError()));
      }
      Mix_PlayMusic(music, -1);
    }
    void StopMusic() {
      if (Mix_PlayingMusic() > 0) {
        Mix_PauseMusic();
      }
    }
    void SetVolume(float vol) {
      volume = vol;
      float logVol = log(1 + volume);
      Mix_VolumeMusic(MIX_MAX_VOLUME * logVol);
    }
    float GetVolume() const {
      return volume;
    }

   public:
    Audio& operator=(const Audio&) = delete;

   private:
    Audio() { music = nullptr; }
    ~Audio() {
      if (Mix_PlayingMusic() > 0)
        StopMusic();
      Mix_CloseAudio();
    }

   private:
    float volume;
    Mix_Music* music;
    inline static Audio* instance = nullptr;
  };

  class Graphics {
   public:
    static bool Init(const char* title, int width, int height, bool fullscreen, bool vsync) {
      Log::Print("Initializing graphics...");

      if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        Log::Error("Unable to initialize SDL.");
        return false;
      }

      window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        0
      );
      if (!window) {
        Log::Error("Unable to create SDL window.");
        return false;
      }

      int rendererIndex = -1;
      for (int i = 0; i < SDL_GetNumRenderDrivers(); i++) {
        SDL_RendererInfo info;
        SDL_GetRenderDriverInfo(i, &info);
        if (strcmp(info.name, "opengl")) {
          rendererIndex = i;
          break;
        }
      }
      if (rendererIndex < 0) {
        Log::Error("OpenGL not found");
        return false;
      }

      renderer = SDL_CreateRenderer(window, rendererIndex, SDL_RENDERER_ACCELERATED);
      if (!renderer) {
        Log::Error("Unable to create SDL renderer.");
        return false;
      }

      SDL_RenderSetLogicalSize(renderer, width, height);
      SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
      SDL_RenderSetVSync(renderer, vsync ? 1 : 0);

      SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(
        0,
        1, 1,
        32, SDL_PIXELFORMAT_RGB888
      );
      SDL_FillRect(surface, nullptr, SDL_MapRGB(
        surface->format,
        255u, 255u, 255u
      ));

      texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_FreeSurface(surface);

      sol::state* lua = Lua::GetInstance().GetState();

      sol::table windowTable = lua->create_table_with(
        "width", width,
        "height", height,
        "title", title,
        "fullscreen", sol::property(&Graphics::IsFullscreen, &Graphics::SetFullscreen)
      );

      (*lua)["scarlet"] = lua->create_table_with(
        "window", windowTable
      );

      Log::Print("Graphics initialized.");

      return true;
    }
    static void Destroy() {
      Log::Print("Destroying graphics related objects...");
      SDL_DestroyTexture(texture);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      SDL_Quit();
    }
    static void PreDraw() {
      SDL_SetRenderDrawColor(renderer, 0u, 0u, 0u, 255u);
      SDL_RenderClear(renderer);
    }
    static void PostDraw() {
      SDL_RenderPresent(renderer);
    }
    static void SetFullscreen(bool on) {
      fullscreen = on;
      SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    }
    static bool IsFullscreen() {
      return fullscreen;
    }
    static SDL_Renderer* GetMainRenderer() { return renderer; }
    static SDL_Texture* GetDefaultTexture() { return texture; }
    static SDL_Window* GetMainWindow() { return window; }

   private:
    Graphics();

   private:
    inline static bool fullscreen = false;
    inline static SDL_Renderer* renderer = nullptr;
    inline static SDL_Texture* texture = nullptr;
    inline static SDL_Window* window = nullptr;
  };

  class Engine {
   public:
    Engine() {
      running = false;
      lua = Lua::GetInstance().GetState();
      now = SDL_GetPerformanceCounter();
      last = 0;
    }
    ~Engine() { if (running) Stop(); }
  
   public:
    bool Init(const char* title, int width, int height, bool fullscreen, bool vsync) {
      Log::Print("Initializing engine...");

      if (!Graphics::Init(title, width, height, fullscreen, vsync)) {
        Log::Error("Failed to initialize Graphics.");
        return false;
      };

      Audio::GetInstance().Init();

      // Had to do this with a lambda for it to work
      (*lua)["scarlet"]["exit"] = [&]() { this->Stop(); };

      Log::Print("Engine initialized.");

      return true;
    }
    bool IsRunning() { return running; }
    void Start() {
      Log::Print("Starting engine...");

      Input::GetInstance().Init();
      Input::GetInstance().FindControllers();

      running = true;
      
      (*lua)["init"] = []() {};
      (*lua)["input"] = [](sol::table) {};
      (*lua)["update"] = [](float) {};
      (*lua)["draw"] = []() {};

      if (std::filesystem::exists("main.lua")) {
        lua->script_file("main.lua");
      }

      (*lua)["init"]();
    }
    void Stop() {
      Log::Print("Stopping engine...");
      Graphics::Destroy();
      running = false;
      Log::Print("Engine stopped.");
    }
    void HandleEvents() {
      SDL_Event event;

      SDL_PumpEvents();

      while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) > 0) {
        switch (event.type) {
          case SDL_QUIT: {
            Log::Print("Quit invoked.");
            Stop();
            break;
          }
          case SDL_CONTROLLERDEVICEADDED: {
            break;
          }
          case SDL_CONTROLLERDEVICEREMOVED: {
            break;
          }
          default: {
            Input::GetInstance().HandleEvent(&event);
            break;
          }
        }
      }

    }
    void Update() {
      last = now;
      now = SDL_GetPerformanceCounter();
      float dt = static_cast<float>(
        (now - last) * 1000 / static_cast<float>(
          SDL_GetPerformanceFrequency()
        )
      ) * 0.001;

      (*lua)["update"](dt);
    }
    void Draw() {
      Graphics::PreDraw();
      (*lua)["draw"]();
      Graphics::PostDraw();
    }

   public:
    sol::state* lua;

   private:
    bool running;
    Uint64 last;
    Uint64 now;
  };
};

#endif // SCARLET_HPP
