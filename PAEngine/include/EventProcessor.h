/*****************************************************************************/
/*
  file: EventProcessor.h
  author: j.mcdonell
  (c) DigiPen Institute of Technology
*/
/*****************************************************************************/
#pragma once

class EventReader;
class EventRegistrationFinder;
class EventSystem;

/* EventProcessor takes Events from the EventQueue, finds and
   calls the fucntion registered to them. */
class EventProcessor {
public:
  /*Creates the EventQueue, Registry, and System for sending messages */
  EventProcessor();
  ~EventProcessor();

  /* processes all the events in the event queue */
  void ProcessEvents();

private:
  EventReader* events_;
  EventRegistrationFinder* registry_;
  EventSystem* eventSystem_;
};