/****************************************************************/
/*
  file: EventQueue.h
  author: j.mcdonell
  (c) DigiPen Institute of Technology
*/
/****************************************************************/
#pragma once

#include <array>
#include "EventSystem.h"

#define EVENT_QUEUE_SIZE 64 // subject to change

// forward declarations
enum EventType;

/* Use this class to send events to the queue */
class EventSender {
public:
	~EventSender() {}
  /* Sends an event to be processed later */
  virtual void SendEvent(EventType type, void* data, unsigned dataSize) = 0;
};

/* data structure to access information about events */
struct Event {
  Event(EventType type = ET_INVALID, void* data = nullptr, unsigned dataSize = 0);

  EventType type_;
  void* data_;
  unsigned dataSize_;
  double timeSent_;
};

/* used by the event processor to read events from the queue */
class EventReader {
public:
	~EventReader() {}
  virtual Event* GetNextEvent() = 0;
  virtual void Reset() = 0;
};


class EventQueue : public EventSender, public EventReader {
public:

  EventQueue();
  virtual ~EventQueue();

  /* Sends an event to be processed later */
  void SendEvent(EventType type, void* data, unsigned dataSize);

  /* gets the next event that should be processed */
  Event* GetNextEvent();
  /* Resets the queue to get it ready for more events */
  void Reset();
private: //members
  std::array<Event, EVENT_QUEUE_SIZE> events_;
  unsigned current_;
  unsigned size_;
};
