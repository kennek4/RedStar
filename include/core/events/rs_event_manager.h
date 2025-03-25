#ifndef RS_EVENT_MANAGER_H
#define RS_EVENT_MANAGER_H

#include "rs_event_listener.h"
#include "rs_events.h"
#include <map>
#include <sys/types.h>
#include <vector>

namespace RS {
class EventManager {
public:
  // Constructor
  EventManager() { _rs_systems.clear(); };

  EventManager(std::map<u_int, std::vector<EventListener *>> rs_systems)
      : _rs_systems(rs_systems) {};

  // Deconstructor
  ~EventManager() {

  };

  // Emits an event to all listeners "subscribed" to the system with the given
  // sid (system ID)
  void emitEvent(u_int sid, const RS_EVENT event);
  bool addListener(u_int sid, EventListener *listener);
  bool removeListener(u_int sid, EventListener *listener);

private:
  std::map<u_int, std::vector<EventListener *>> _rs_systems;
};
} // namespace RS

#endif // !RS_EVENT_MANAGER_H
