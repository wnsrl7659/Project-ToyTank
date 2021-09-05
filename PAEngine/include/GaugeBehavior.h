/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/
#pragma once
#pragma once
#include "Behavior.h"
#include "InputDevices.h" // gamepad

/***************************************************************************/
/*  Forward declarations                                                   */
class ChangeT; //  movement controller

class GaugeBehavior : public Behavior {
public:
  //constructor should be used to add functions to the base class's function lists
  GaugeBehavior();
  ~GaugeBehavior();

  //  use Create and Destroy to initialize and clear values specific to each Gauge
  void Create(int parentID);
  void Destroy();
  int ID = 0;
  int targetObjectID;
  float initialScaleX = 5;
  float initialScaleY = 0.2f;

private:
  // States used in the state machine
  enum GaugeBehaviorState { // change anything you need to, might need to move to public
    FBS_INVALID = -1,
    FBS_NEUTRAL,
  };

  //  private data members
  PAEngine::Gamepad* gamepad_;
  ChangeT* forces_; //  used to move Gauges transform

  friend class GaugeBehavStateDefault;

  void GaugeUpdate();

};
