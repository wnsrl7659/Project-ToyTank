/******************************************************************************
  file: PlayerBehavStateDead.h
  author: m.kang
  (c) DigiPen Institute of Technology
******************************************************************************/

#pragma once
#include "BehaviorState.h"

class PlayerBehavior;

class PlayerBehavStateDead :
  public BehaviorState
{
public:
  PlayerBehavStateDead(PlayerBehavior* playerBehavior) : playerBehavior_(playerBehavior) {};
  ~PlayerBehavStateDead() {};

  void Init() override;
  void Update(float dt) override;
  void Shutdown() override;

  void TerminateThisState();

private:
  PlayerBehavior* playerBehavior_;
};

