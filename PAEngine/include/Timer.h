/**
 * @file Timer.h
 *
 * @brief It can check time and track "deltaTime" between frames.
 *        This class is refered from bell0bytes.eu
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

namespace Util
{
  class Timer
  {
  public:
  // constructor
  Timer();
  ~Timer();

  // getters: return time measured in seconds
  double GetTotalTime() const;  // returns the total time the game has been running (minus paused time)
  double GetDeltaTime() const;  // returns the time between two frames

  // methods
  void Start();  // starts the timer, called each time the game is unpaused
  void Reset();  // sets the counter to zero, called once before message loop
  void Tick();  // called every frame, lets the time tick
  void Stop();  // called when the game is paused

  private:
  // times measured in counts
  long long int startTime;  // time at the start of the application
  long long int totalIdleTime;  // total time the game was idle
  long long int pausedTime;  // time at the moment the game was paused last
  long long int currentTime;  // stores the current time; i.e. time at the current frame
  long long int previousTime;  // stores the time at the last inquiry before current; i.e. time at the previous frame

  // times measured in seconds
  double secondsPerCount;  // reciprocal of the frequency, computed once at the initialization of the class
  double deltaTime;    // time between two frames, updated during the game loop

  // state of the timer
  bool isStopped;    // true iff the timer is stopped
  };
}