/******************************************************************************
  file: EventRegistry.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#include <array>
#include "EventSystem.h"

/* used to Register events, sets the function to be called when an event 
   with the event type is processed     */
class EventRegistry {
public:
  virtual void RegisterEvent(EventType type, EventHandler function) = 0;
};

/* EventProcessor uses this to find functions registered to event types */
class EventRegistrationFinder {
public:
  virtual EventHandler GetFunctionForEvent(EventType type) const = 0;
};


class EventRegistryImplementation : public EventRegistry, public EventRegistrationFinder {
public:
  EventRegistryImplementation();
  virtual ~EventRegistryImplementation();

  /* Regester functions to handle Events */
  void RegisterEvent(EventType type, EventHandler function);
  /* Gets the function for a registered event type */
  EventHandler GetFunctionForEvent(EventType type) const;
private:
  std::array<EventHandler, ET_REGISTRY_SIZE> registry_;
};
