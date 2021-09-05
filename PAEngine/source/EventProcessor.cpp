/****************************************************************/
/*
  file: EventProcessor.cpp
  author: j.mcdonell
  (c) DigiPen Institute of Technology
*/
/****************************************************************/
#include "pch.h"
#include "EventProcessor.h"
#include "EventQueue.h"
#include "EventRegistry.h"
#include "ServiceLocator.h"

EventProcessor::EventProcessor() {
  events_ = new EventQueue();
  registry_ = new EventRegistryImplementation();
  eventSystem_ = new EventSystem(
     dynamic_cast<EventRegistry*>(registry_),
     dynamic_cast<EventSender*>(events_));
}

EventProcessor::~EventProcessor() {
  //delete events_;
  //delete registry_;
  delete eventSystem_;
}

static void ProcessEvent(Event* toProcess, EventRegistrationFinder* registration) {
  // getting the function registered to the event
  EventHandler function = registration->GetFunctionForEvent(toProcess->type_);
  
  // error checking on the function
  if (function == nullptr) {
  Util::ServiceLocator::getLogger()->print(Util::SeverityType::error, "Processing Unegistered event\n");
  return;
  }

  // finish processing event by calling the function
  function(toProcess->data_);
}

void EventProcessor::ProcessEvents() {
  Event* currentEvent = nullptr;
  while (currentEvent = events_->GetNextEvent()) {
  ProcessEvent(currentEvent, registry_);
  }
  events_->Reset(); // get the queue ready for the next set of events
}
