
#include "rs_render.h"
#include "rs_event_manager.h"
#include "rs_events.h"

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
