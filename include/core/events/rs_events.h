#ifndef RS_EVENTS_H
#define RS_EVENTS_H

#include <any>
namespace RS {

typedef enum RS_EVENT {
  // General Events
  RS_EVENT_NULL, // Does not represent anything

  // Window System Events
  RS_EVENT_WINDOW_VISIBLE,
  RS_EVENT_WINDOW_HIDDEN,
  RS_EVENT_WINDOW_RESIZED,

} RS_EVENT;

struct EventData {
  RS_EVENT eventType;
  std::any data;
};

} // namespace RS

#endif // !RS_EVENTS_H
