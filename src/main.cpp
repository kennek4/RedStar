#include "core/engine.h"

int main(int argc, char *argv[]) {
  RS::Engine *engine = new RS::Engine(0, 1, 0);
  delete engine;
  return 0;
};
