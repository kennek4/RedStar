#ifndef RS_ENGINE_H
#define RS_ENGINE_H

#include "rs_event_manager.h"
#include "rs_input.h"
#include "rs_render.h"
#include "rs_system.h"
#include "rs_window.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <cstdio>
#include <string>
#include <sys/types.h>
#include <vector>

namespace RS {
class Engine {
public:
  bool isRunning;
  Engine(u_int major = 0, u_int minor = 0, u_int patch = 0) {
    _major_ver = major;
    _minor_ver = minor;
    _patch_ver = patch;

    setMetaData();
    initSubSystems();
    isRunning = true;
  };

  ~Engine() {
    delete _input_system;
    delete _render_system;
    delete _window_system;
    delete _event_manager;
  };

  inline const char *getEngineVersion() { return _engine_ver.c_str(); };
  inline SDL_Window *getWindow() { return _window_system->getWindow(); };

  // Event Loop Methods
  void handleInput();
  void update();
  void renderFrame();

private:
  void setMetaData() {
    // Current _engine_ver
    _engine_ver += std::to_string(_major_ver) + ".";
    _engine_ver += std::to_string(_minor_ver) + ".";
    _engine_ver += std::to_string(_patch_ver);
    // Do other metadata things here
  };

  // TODO: Add custom error handling, change from bool to custom error type

  // Returns true if all subsystems were initialized, false if there was an
  // error.
  bool initSubSystems() {
    if (_window_system != NULL || _render_system != NULL ||
        _event_manager != NULL) {
      // One of the pointers to a system is corrupt,
      // We should call for the exit of the program here.
      return false;
    }

    _event_manager = new EventManager();
    _window_system = new WindowSystem(_event_manager, 1);

    // TODO: Add error handling if system not initialized, exit
    _window_system->initSDL(_engine_ver.c_str());
    _initialized_systems.push_back(_window_system);

    _render_system = new RenderSystem(_event_manager, 2);
    _initialized_systems.push_back(_render_system);

    _input_system = new InputSystem(_event_manager, 3);
    _initialized_systems.push_back(_input_system);

    return true;
  }; // TODO: Switch from bools to custom Error type

  // Engine metadata
  u_int _major_ver;
  u_int _minor_ver;
  u_int _patch_ver;
  std::string _engine_ver;

  // Systems
  EventManager *_event_manager;
  WindowSystem *_window_system;
  RenderSystem *_render_system;
  InputSystem *_input_system;

  std::vector<System *> _initialized_systems;

  SDL_Event _event;
};
} // namespace RS

#endif // !RS_ENGINE_H
