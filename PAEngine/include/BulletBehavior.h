/******************************************************************************
  file: BehaviorManager.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/

#pragma once
#include "Behavior.h"

/***************************************************************************/
/*  Forward declarations                                                   */

class BulletBehavior : public Behavior {
public:
  //constructor should be used to add functions to the base class's function lists
  BulletBehavior();
  ~BulletBehavior();

  //  use Create and Destroy to initialize and clear values specific to each Bullet
  void Create(int idOfShooter, bool explode = false, bool damagePlayer = true);
  void Destroy();
  void SetBulletParameters(bool explode, bool damagePlayer);
  bool getExplode();
  bool WillDamagePlayer();
  void Update();
  int GetShooterID() const;
  void SetTargetID(int tid);
  float ttl_ = 5;
  bool hit = false;

private:
  // States used in the state machine
  enum BulletBehaviorState  // change anything you need to, might need to move to public
  { 
    BBS_SHOT
  };

  int shooterID_;
  int targetID_ = -1;
  bool explode_ = false;
  bool damagePlayer_ = true;

  friend class BulletBehaveShotState;
};