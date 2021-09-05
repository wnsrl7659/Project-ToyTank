/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/
#pragma once
#include "Behavior.h"
#include "InputDevices.h" // gamepad

/***************************************************************************/
/*  Forward declarations                                                   */
class ChangeT; //  movement controller

class AIBehavior : public Behavior {
public:
	//constructor should be used to add functions to the base class's function lists
	AIBehavior();
	~AIBehavior();

	//  use Create and Destroy to initialize and clear values specific to each AI
	void Create(int parentID);
	void Destroy();

  inline void SetPartIDs(const int _headID, const int _bodyID) {
    partHeadID = _headID;  partBodyID = _bodyID; }
  void SetColor(int id);
  void SetOwner(int id);
  int GetOwner();
  void DecrementHP();
  void ResetHP();
  int GetHP();
  const int GetRange();
  void SetTarget(int id);
  int GetTarget();

  // yaw: horizontal direction of radian
  float GetHeadDirectionYaw() { return currentAimRadianX; }
  // pitch: vertical direction of radian
  float GetHeadDirectionPitch() { return currentAimRadianY; }
  // move the direction by the given raidan (yaw, pitch)
  void SetHeadDirection(const float horizontalRadian, const float verticalRadian);
  // move the direction to the given target position
  // parameters are the position of Transform compoment
  void SetHeadDirection(const float _x, const float _y, const float _z);
  bool moveXaxis = true;
  
private:
	// States used in the state machine
	enum AIBehaviorState { // change anything you need to, might need to move to public
		PBS_INVALID = -1,
		PBS_PLAYING,
		PBS_DYING
	};

	//  private data members
	PAEngine::Gamepad* gamepad_;
	ChangeT* forces_; //  used to move AIs transform

  int counter = 0;
  float moveX = 0;
  float moveZ = 0;
  float time_ = 0;

  int owner = -1;
  int HP = 5;
  int range = 1500;
  int target = -1;
  int targetID = -1;
  float speed = .2f;
  

  // for head rotation (target tracking)
  int partHeadID;
  int partBodyID;
  float currentAimRadianX; // target yaw
  float currentAimRadianY; // target pitch
  float currentAimUnitVecX_X;
  float currentAimUnitVecX_Z;
  float currentAimUnitVecY_Y;
  float currentAimUnitVecY_Z;
  void HeadRotate();

	friend class AIBehavStateDefault;

	void DefaultUpdate();

};