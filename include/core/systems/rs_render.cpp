
#include "rs_render.h"
#include "rs_event_manager.h"
#include "rs_events.h"
#include <GL/gl.h>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>

inline void ::RS::RenderSystem::emitEvent(const RS_EVENT event) {
  // Do nothing
  if (_event_manager == NULL) {
    return;
  }
  _event_manager->emitEvent(_sid, event);
};

inline void ::RS::RenderSystem::update(const RS_EVENT event) {
  _last_event = event;
};

void ::RS::RenderSystem::initOpenGL() {
  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);

  glBindVertexArray(_vao);
};
