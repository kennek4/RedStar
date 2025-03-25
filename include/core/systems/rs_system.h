#ifndef RS_SYSTEM_H
#define RS_SYSTEM_H

#include "rs_event_listener.h"
#include "rs_events.h"
#include <sys/types.h>

namespace RS {
class System : public EventListener {
public:
  // TODO: Change emitEvent return type to handle custom errors
  // Used to broadcast events to listeners of the current system
  virtual void emitEvent(const RS_EVENT event) = 0;

  // TODO: Change update return type to handle custom errors
  virtual void update(const RS_EVENT event) override = 0;

private:
};
} // namespace RS

#endif // !RS_SYSTEM_H
