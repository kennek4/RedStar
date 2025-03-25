#include "rs_window.h"
#include "rs_event_manager.h"
#include "rs_events.h"
#include <GL/gl.h>
#include <GLES2/gl2.h>
#include <SDL3/SDL_video.h>

void ::RS::WindowSystem::emitEvent(const RS_EVENT event) {
  // Do nothing
  if (_event_manager == NULL) {
    return;
  }

  _event_manager->emitEvent(_sid, event);
};

inline void ::RS::WindowSystem::update(const RS_EVENT event) {
  _last_event = event;
};

bool ::RS::WindowSystem::initSDL(const char *engineVersion) {

  // TODO: Get WIN_WIDTH, WIN_HEIGHT, and CreateWindow FLAGS
  //       from a User Config file(?)

  // Temp hard code
  const char *_PROGRAM_NAME = "RedStar";
  const SDL_WindowFlags _FLAGS = SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS;
  int _WIN_WIDTH = 1600;
  int _WIN_HEIGHT = 900;

  const int _NUM_OF_SUBSYSTEMS = 2;
  const std::pair<unsigned int, const char *> SUBSYSTEMS[_NUM_OF_SUBSYSTEMS]{
      {SDL_INIT_VIDEO, "VIDEO"}, {SDL_INIT_AUDIO, "AUDIO"}};

  SDL_SetAppMetadata(_PROGRAM_NAME, engineVersion, NULL);

  // Start all subsystems inorder
  for (int i = 0; i < _NUM_OF_SUBSYSTEMS; i++) {
    if (SDL_InitSubSystem(SUBSYSTEMS[i].first) == false) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s SUBSYSTEM FAILED TO INIT: %s\n",
                   SUBSYSTEMS[i].second, SDL_GetError());
      return false;
    }
  };

  // Init OpenGL stuff
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  _window = SDL_CreateWindow(_PROGRAM_NAME, _WIN_WIDTH, _WIN_HEIGHT, _FLAGS);
  if (_window == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "THE PROGRAM WINDOW FAILED TO INIT: %s\n", SDL_GetError());
    return false;
  };

  _gl_context = SDL_GL_CreateContext(_window);
  if (_gl_context == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "OpenGL CONTEXT COULD NOT BE CREATED\n");
    return false;
  };

  _gl_program_id = glCreateProgram();

  // Set SDL Settings
  // VSync: 0 for off, 1 for on, -1 for adaptive
  SDL_GL_SetSwapInterval(1);
  return true;
};
