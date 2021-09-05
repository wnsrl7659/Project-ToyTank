/******************************************************************************
  file: EventRegistry.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "EventRegistry.h"
#include "EventSystem.h"
#include "ServiceLocator.h"

EventRegistryImplementation::EventRegistryImplementation()
  : registry_()
{}

EventRegistryImplementation::~EventRegistryImplementation(){
}

void EventRegistryImplementation::RegisterEvent(EventType type, EventHandler function) {
  //error checking
  if (type >= ET_REGISTRY_SIZE || type < 0) {
  Util::Logger log;
  log.print(Util::SeverityType::error, "Invalid EventType Registered\n");
  return;
  }
  else if (registry_[type] != nullptr) {
  Util::Logger log;
  log.print(Util::SeverityType::error, "Overwriting registered event\n");
  return;
  }
  
  //sets the function to be called when the event is processed
  registry_[type] = function;
}

EventHandler EventRegistryImplementation::GetFunctionForEvent(EventType type) const {
  // error checking
  if (type >= ET_REGISTRY_SIZE || type < 0) {
  Util::ServiceLocator::getLogger()->print(Util::SeverityType::error, "Getting function for invalid event type\n");
  return nullptr;
  }
  else if (registry_[type] == nullptr) {
  Util::ServiceLocator::getLogger()->print(Util::SeverityType::error, "Getting Function for unregistered event\n");
  return nullptr;
  }

  //returns teh function registered to this event
  return registry_[type];
}

