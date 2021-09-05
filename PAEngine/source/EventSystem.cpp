/******************************************************************************
  file: EventSystem.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "EventSystem.h"
#include "EventQueue.h"
#include "EventRegistry.h"

EventSystem* EventSystem::thisSystem_ = nullptr;

EventSystem::EventSystem(EventRegistry* registry, EventSender* eventQueue)
  : eventQueue_(eventQueue), registry_(registry) {
  if (thisSystem_ == nullptr) {
  thisSystem_ = this;
  }
}
EventSystem::~EventSystem() {
  thisSystem_ = nullptr;
}

void EventSystem::SendEvent(EventType type, void* data, unsigned dataSize) {
  thisSystem_->eventQueue_->SendEvent(type, data, dataSize);
}

void EventSystem::RegisterEvent(EventType type, EventHandler function) {
  thisSystem_->registry_->RegisterEvent(type, function);
}
