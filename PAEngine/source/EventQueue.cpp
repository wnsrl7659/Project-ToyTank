/****************************************************************/
/*
  file: EventQueue.cpp
  author: j.mcdonell
  (c) DigiPen Institute of Technology
*/
/****************************************************************/
#include "pch.h"
#include "EventQueue.h"
#include "EventRegistry.h"
#include "Timer.h"
#include "App.h"


EventQueue::EventQueue()
  : events_({}), current_(0), size_(0) {
}

EventQueue::~EventQueue(){
}

void EventQueue::SendEvent(EventType type, void* data, unsigned dataSize) {
  if (PAEngine::App::globalApp->isPaused && type >= ET_INPUT_LOOK_UP_StillPressed && type <= ET_SOUND_L_END_MENUMUSIC) {
    return;
  }
  if (size_ >= events_.max_size())
    printf("SendEvent, Out of Index!!");
  else
    events_[size_++] = Event(type, data, dataSize);
}

Event* EventQueue::GetNextEvent() {
  if (current_ < size_) {
  return &events_[current_++];
  }
  else {
  return nullptr;
  }
}

void EventQueue::Reset() {
  size_ = 0;
  current_ = 0;
}

Event::Event(EventType type, void* data, unsigned dataSize)
  : type_(type), data_(data), dataSize_(dataSize), timeSent_(0) {
}
