/******************************************************************************
  * Author : Cody Morgan
  * Class  : GAM 300
  * Brief  : performance monitoring stuff and unit testing

  (c) DigiPen Institute of Technology 2019
******************************************************************************/
#ifndef PERFORMANCE_H
#define PERFORMANCE_H
#pragma once

#include <chrono>

class StopWatch
{
public:
  enum class PrintMode
  {
    PrintDefault,
    PrintMicroSeconds,
    PrintMilliSeconds,
    PrintSeconds,
  };

  void start(unsigned average = 10);
  void tick(PrintMode mode = PrintMode::PrintMicroSeconds);
  void end();
  unsigned getSeconds();
  unsigned getMilliseconds();
  unsigned getMicroseconds();
  float getAverage();
  void printElapsed(PrintMode mode = PrintMode::PrintDefault);

private:
  std::chrono::time_point<std::chrono::steady_clock> start_;
  std::chrono::time_point<std::chrono::steady_clock> end_;
  std::vector<unsigned>times_;
  unsigned frame_ = 0;
  unsigned averageCount_;
  bool isRunning = false;
};

#endif
