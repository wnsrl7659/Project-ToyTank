/******************************************************************************
  * Author : j.mcdonell
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once
#include "Behavior.h"
#include "InputDevices.h" // gamepad

class ChangeT; //  movement controller


class TankBehavior : public Behavior {
public:

  TankBehavior()
    : gamepad_(nullptr), forces_(nullptr), partHeadID(-1), partBodyID(-1) {};
  ~TankBehavior() {};

  inline void SetPartIDs(const int _headID, const int _bodyID) {
    partHeadID = _headID;  partBodyID = _bodyID;
  }
 
private:

  //  private data members
  PAEngine::Gamepad* gamepad_;
  ChangeT* forces_; //  used to move players transform

  int partHeadID;
  int partBodyID;

  float currentAimRadianX;
  float currentAimRadianY;
};