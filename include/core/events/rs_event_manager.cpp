#include "rs_event_manager.h"
#include "rs_event_listener.h"
#include "rs_events.h"
#include <sys/types.h>
#include <vector>

void ::RS::EventManager::emitEvent(u_int sid, const RS_EVENT event) {
  // First get the EventListeners subscribed to
  // the system with sid == sid
  std::vector<EventListener *> _listeners = _rs_systems[sid];
  for (int i = 0; i < _listeners.size(); i++) {
    _listeners[i]->update(event);
  };
};

bool ::RS::EventManager::addListener(u_int sid, EventListener *listener) {
  _rs_systems[sid].push_back(listener);

  // listener was not placed added into the vector
  if (_rs_systems[sid].back() != listener) {
    return false; // TODO: Add custom error handling
  };

  return true;
};

bool ::RS::EventManager::removeListener(u_int sid, EventListener *listener) {
  for (u_int i = 0; i < _rs_systems[sid].size(); i++) {
    if (_rs_systems[sid].at(i) == listener) {
      _rs_systems[sid].erase(_rs_systems[sid].begin() + i);
      return true;
    };
  };

  // TODO: Custom error handling for if removing fails
  return false;
};
