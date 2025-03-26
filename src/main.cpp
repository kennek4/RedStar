#include "core/engine.h"

int main(int argc, char *argv[]) {
  RS::Engine *engine = new RS::Engine(0, 1, 0);

  while (engine->isRunning) {
    engine->handleInput();
    engine->update();
    engine->renderFrame();
  }

  delete engine;
  return 0;
};
