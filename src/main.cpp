#include <GL/gl.h>
#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengles2_gl2.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <utility>

#define NUM_OF_SUBSYSTEMS 3
#define PROGRAM_NAME "RedStar"
#define PROGRAM_VERSION "0.1.0"

static int winWidth = 1280;
static int winHeight = 720;
static SDL_Window *programWin = NULL;
static SDL_Renderer *programRenderer = NULL;
static SDL_GLContext gContext = NULL;
static GLuint gProgramID = 0;

const std::pair<unsigned int, const char *> SUBSYSTEMS[NUM_OF_SUBSYSTEMS]{
    {SDL_INIT_VIDEO, "VIDEO"}, {SDL_INIT_AUDIO, "AUDIO"}};

// Initializes all subsystems declared in the SUBSYSTEMS pair array
// Will return false if any subsystem fails, returns true otherwise
bool startProgram() {
  SDL_SetAppMetadata(PROGRAM_NAME, PROGRAM_VERSION, NULL);

  // Start all subsystems inorder
  for (int i = 0; i < NUM_OF_SUBSYSTEMS; i++) {
    if (SDL_InitSubSystem(SUBSYSTEMS[i].first) == false) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s SUBSYSTEM FAILED TO INIT: %s\n",
                   SUBSYSTEMS[i].second, SDL_GetError());
      return false;
    }
  }

  // Init OpenGL stuff
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  /*SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Anti ALiasing 4x*/

  programWin =
      SDL_CreateWindow(PROGRAM_NAME, winWidth, winHeight, SDL_WINDOW_OPENGL);
  if (programWin == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "THE PROGRAM WINDOW FAILED TO INIT: %s\n", SDL_GetError());
    return false;
  }

  gContext = SDL_GL_CreateContext(programWin);
  if (gContext == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                 "OpenGL CONTEXT COULD NOT BE CREATED\n");
    return false;
  }

  // VSync: 0 for off, 1 for on, -1 for adaptive
  SDL_GL_SetSwapInterval(1);
  gProgramID = glCreateProgram();
  return true;
}

void endProgram() {
  // Close things before exiting
  SDL_GL_DestroyContext(gContext);
  SDL_DestroyRenderer(programRenderer);
  SDL_DestroyWindow(programWin);
  SDL_Quit();
}

int main(int argc, char *argv[]) {
  // If any subsystem fails to initialize, end the program
  if (!startProgram()) {
    endProgram();
  }

  SDL_Event event;
  bool finished = false;

  while (!finished) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_KEY_DOWN &&
          event.key.scancode == SDL_SCANCODE_ESCAPE) {
        finished = true;
      }

      glViewport(0, 0, winWidth, winHeight);
      glClearColor(1.f, 0.f, 0.f, 0.f);
      glClear(GL_COLOR_BUFFER_BIT);
      SDL_GL_SwapWindow(programWin);
    }
  }

  endProgram();
}
