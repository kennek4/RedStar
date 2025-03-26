#ifndef RS_WINDOW_H
#define RS_WINDOW_H

#include "rs_event_manager.h"
#include "rs_events.h"
#include "rs_system.h"
#include <GL/gl.h>
#include <SDL3/SDL.h>
#include <sys/types.h>

namespace RS {
class WindowSystem : public System {
public:
  // Constructor
  WindowSystem(EventManager *eventManager, u_int sid) {
    _event_manager = eventManager;
    _sid = sid;
    _last_event = RS_EVENT_NULL;
  };

  // Deconstructor
  ~WindowSystem() {
    // TODO: Error handle on destroy fail
    SDL_GL_DestroyContext(_gl_context);
    SDL_DestroyWindow(_window);
    SDL_Quit();
  };

  void emitEvent(const RS_EVENT event) override;
  inline void update(const RS_EVENT event) override;
  // Getters
  inline SDL_Window *getWindow() { return _window; };
  inline GLuint getGLProgramID() { return _gl_program_id; };

  // Setters

  // SDL3
  // Starts up all necessary SDL3 processes
  bool initSDL(const char *engineVer);

private:
  RS_EVENT _last_event;
  EventManager *_event_manager;
  u_int _sid;
  bool _initialized;

  // SDL3
  int _window_w;
  int _window_h;
  SDL_Window *_window;

  // OpenGL
  SDL_GLContext _gl_context;
  GLuint _gl_program_id;
};
} // namespace RS

#endif // !RS_WINDOW_H
