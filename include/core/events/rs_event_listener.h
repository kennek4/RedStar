#ifndef RS_EVENT_LISTENER_H
#define RS_EVENT_LISTENER_H

#include "rs_events.h"
namespace RS {
class EventListener {
public:
  virtual void update(const RS_EVENT event) = 0;

private:
};
} // namespace RS

#endif // !RS_EVENT_LISTENER_H
