#include "core/engine.h"
#include <GL/gl.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_video.h>

void Update() {}

int main(int argc, char *argv[]) {
  RS::Engine *engine = new RS::Engine(0, 1, 0);

  bool exit = false;
  SDL_Event event;
  while (!exit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_KEY_DOWN &&
          event.key.scancode == SDL_SCANCODE_ESCAPE) {
        exit = true;
      };
    };

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(engine->getWindow());
  };

  delete engine;
  return 0;
};
