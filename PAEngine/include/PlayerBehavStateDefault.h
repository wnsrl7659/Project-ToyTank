/******************************************************************************
  file: PlayerBehavStateDefault.h
  author: j.mcdonell
  (c) DigiPen Institute of Technology
******************************************************************************/

#pragma once
#include "BehaviorState.h"

class PlayerBehavior;

class PlayerBehavStateDefault :
  public BehaviorState
{
public:
  PlayerBehavStateDefault(PlayerBehavior* playerBehavior) : playerBehavior_(playerBehavior) {};
  ~PlayerBehavStateDefault() {};

  void Init() override;
  void Update(float dt) override;
  void Shutdown() override;

private:
  PlayerBehavior* playerBehavior_;
};

