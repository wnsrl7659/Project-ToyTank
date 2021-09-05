#include "pch.h"
#include "PerformanceMonitor.h"
#include <iostream>
using std::cout;

void StopWatch::start(unsigned average)
{
  times_.clear();
  times_.resize(average);
  averageCount_ = average;

  start_ = std::chrono::steady_clock::now();
}

// capture current time - but keep it running for averaging
void StopWatch::tick(StopWatch::PrintMode mode)
{
  if (isRunning)
  {
    end();
    // update times
    unsigned time = 0;
    switch (mode)
    {
    case StopWatch::PrintMode::PrintDefault:
    case StopWatch::PrintMode::PrintMilliSeconds:
      time = getMilliseconds();
      break;

    case StopWatch::PrintMode::PrintMicroSeconds:
      time = getMicroseconds();
      break;

    case StopWatch::PrintMode::PrintSeconds:
      time = getSeconds();
      break;

    default:
      break;
    }
    frame_ %= averageCount_;
    times_[frame_] = time;
    frame_++;
  }

  // not running so start it 
  else
  {
    start_ = std::chrono::steady_clock::now();
  }

  isRunning = !isRunning;
}

void StopWatch::end()
{
  end_ = std::chrono::steady_clock::now();
}

unsigned StopWatch::getMilliseconds()
{
  return unsigned(std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_).count());
}

unsigned StopWatch::getMicroseconds()
{
  return unsigned(std::chrono::duration_cast<std::chrono::microseconds> (end_ - start_).count());

}

float StopWatch::getAverage()
{
  float avg = 0;
  for (unsigned time : times_)
    avg += time;

  return avg / averageCount_;
}

unsigned StopWatch::getSeconds()
{
  return unsigned(std::chrono::duration_cast<std::chrono::seconds>(end_ - start_).count());
}

void StopWatch::printElapsed(StopWatch::PrintMode mode)
{
  cout << "\nElapsed Time: ";
  switch (mode)
  {
  case StopWatch::PrintMode::PrintMilliSeconds:
    cout << getMilliseconds() << " ms\n";
    break;

  case StopWatch::PrintMode::PrintSeconds:
    cout << getSeconds() << " sec\n";
    break;

  default:
    if (getSeconds() > 0)
      cout << getSeconds() << " seconds\n";
    else if (getMilliseconds() > 0)
      cout << getMilliseconds() << " milliseconds\n";
    else
      cout << getMicroseconds() << " microseconds\n";
    break;
  }
}
