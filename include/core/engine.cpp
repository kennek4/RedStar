#include "core/engine.h"
#include <SDL3/SDL_log.h>

void ::RS::Engine::handleInput() {

  // TODO: Use InputSystem here

  while (SDL_PollEvent(&_event)) {
    if (_event.type == SDL_EVENT_KEY_DOWN &&
        _event.key.scancode == SDL_SCANCODE_ESCAPE) {
      isRunning = false;
    };
  };
};

void ::RS::Engine::update(){
    //
};

void ::RS::Engine::renderFrame() {
  glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  SDL_GL_SwapWindow(_window_system->getWindow());
};
