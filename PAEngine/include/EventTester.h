/******************************************************************************
  * Author : j.mcdonell
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once

#include "Log.h"
class EventTester {
public:
  EventTester();
  ~EventTester();

  void RegisterTest();
  void RegisterInvalidType();
  void RegisterInvalidFunction();
  void Send();
  void SendInvalid();
  void SendBadData();
  void Reregister();
private:
  Util::Logger log;
};