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

class PowerUpsBehavior : public Behavior {
public:
  //constructor should be used to add functions to the base class's function lists
  PowerUpsBehavior();
  ~PowerUpsBehavior();

  //  use Create and Destroy to initialize and clear values specific to each PowerUps
  void Create(int parentID);

  void Update(float dt);
  void Destroy();
  bool finishGame = true;

  int powerUpType = -1;
  // 0 - HP Recover
  // 1 - FireRate++
  // 2 - Invulnerability
  // 3 - Speed++
  // 4 - Speed--

private:

  // States used in the state machine
  enum PowerUpsBehaviorState { // change anything you need to, might need to move to public
    PBS_INVALID = -1,
    PBS_NEUTRAL,
    PBS_HELD


  };

  //  private data members
  PAEngine::Gamepad* gamepad_;
  ChangeT* forces_; //  used to move PowerUpss transform

  friend class PowerUpsBehavStateDefault;

  void PowerUpsUpdate();

  unsigned updateDelay = 5;
  unsigned updateDelayCount = 0;
};