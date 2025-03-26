#ifndef RS_INPUT_H
#define RS_INPUT_H

#include "rs_event_manager.h"
#include "rs_system.h"
#include <SDL3/SDL_events.h>
#include <cstddef>

namespace RS {
class InputSystem : public System {
public:
  InputSystem(EventManager *eventManager, u_int sid) {
    _event_manager = eventManager;
    _sid = sid;
  };

  ~InputSystem() { _event_manager = NULL; };

  void emitEvent(const RS_EVENT event) override {};
  void update(const RS_EVENT event) override {};

private:
  EventManager *_event_manager;
  u_int _sid;
  bool isInitialized;
};
} // namespace RS

#endif // !RS_INPUT_H
