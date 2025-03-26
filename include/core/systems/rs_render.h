#ifndef RS_RENDER_H
#define RS_RENDER_H

#include "rs_event_manager.h"
#include "rs_events.h"
#include "rs_system.h"
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <sys/types.h>

namespace RS {
class RenderSystem : public System {
public:
  // Constructor
  RenderSystem(EventManager *eventManager, u_int sid) {
    _event_manager = eventManager;
    _sid = sid;
    _last_event = RS_EVENT_NULL;
    initOpenGL();
  };

  // Deconstructor
  ~RenderSystem() {
    _event_manager = NULL;

    // TODO: Handle unintialiized vaos, vbos, or ebos
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
  };

  void emitEvent(const RS_EVENT event) override;
  void update(const RS_EVENT event) override;

  // Start all necessary OpenGL processes
  void initOpenGL();

private:
  RS_EVENT _last_event;
  EventManager *_event_manager;
  u_int _sid;
  bool isInitialized;

  // OpenGL
  u_int _vbo;
  u_int _vao;
  u_int _ebo;
};
} // namespace RS

#endif // !RS_RENDER_H
