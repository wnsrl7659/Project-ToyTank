#include "pch.h"
#include "EventTester.h"
#include "EventSystem.h"
#include "Log.h"
#include <assert.h>

struct testData {
  int i;
  int j;
};

static void testHandler(testData*data) {
  assert(data->j == 2);
  delete data;
}

EventTester::EventTester() : log() {
  log.print(Util::SeverityType::info, "Event Tester Initialized\n");
}

EventTester::~EventTester() {}

void EventTester::RegisterTest() {
  log.print(Util::SeverityType::info, "Registering test function\n");
  EventSystem::RegisterEvent(ET_TEST, (EventHandler)testHandler);
}

void EventTester::RegisterInvalidType()
{
  log.print(Util::SeverityType::info, "Registering test function to invalid event type\n");
  EventSystem::RegisterEvent(ET_INVALID, (EventHandler)testHandler);
}

void EventTester::RegisterInvalidFunction() {
  EventSystem::RegisterEvent(ET_TEST, nullptr);
}

void EventTester::Send() {
  testData* test = new testData;
  test->i = 1;
  test->j = 2;
  log.print(Util::SeverityType::info, "Sending valid event\n");
  EventSystem::SendEvent(ET_TEST, test, sizeof(testData));
}

void EventTester::SendInvalid() {
  log.print(Util::SeverityType::info, "Sending invalid Event\n");
  EventSystem::SendEvent(ET_INVALID, nullptr, 0);
}

void EventTester::SendBadData() {
  log.print(Util::SeverityType::info, "Sending event with bad data\n");
  EventSystem::SendEvent(ET_TEST, nullptr, 0);
}


void EventTester::Reregister() {
  log.print(Util::SeverityType::info, "Registering test event again\n");
  EventSystem::RegisterEvent(ET_TEST, (EventHandler)testHandler);
}
