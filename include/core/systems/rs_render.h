#ifndef RS_RENDER_H
#define RS_RENDER_H

#include "rs_event_manager.h"
#include "rs_events.h"
#include "rs_system.h"
#include <sys/types.h>

namespace RS {
class RenderSystem : public System {
public:
  // Constructor
  RenderSystem(EventManager *eventManager, u_int sid) {
    _event_manager = eventManager;
    _sid = sid;
    _last_event = RS_EVENT_NULL;
  };

  RenderSystem() {
    _event_manager = NULL;
    _last_event = RS_EVENT_NULL;
  };

  // Deconstructor
  ~RenderSystem() { _event_manager = NULL; };

  void emitEvent(const RS_EVENT event) override;
  void update(const RS_EVENT event) override;

private:
  RS_EVENT _last_event;
  EventManager *_event_manager;
  u_int _sid;
  bool isInitialized;
};
} // namespace RS

#endif // !RS_RENDER_H
