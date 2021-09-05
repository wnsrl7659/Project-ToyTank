/******************************************************************************
  file: PlayerBehavStateRapidFire.h
  author: j.mcdonell
  (c) DigiPen Institute of Technology
******************************************************************************/

#pragma once
#include "BehaviorState.h"

class PlayerBehavior;

class PlayerBehavStateRapidFire : public BehaviorState{
public:
  PlayerBehavStateRapidFire(PlayerBehavior* playerBehavior) : playerBehavior_(playerBehavior) {}
  ~PlayerBehavStateRapidFire() {}

  void Init() override;
  void Update(float dt) override;
  void Shutdown() override;

private:
  PlayerBehavior* playerBehavior_;
};

class PlayerBigShot : public BehaviorState {
public:
  PlayerBigShot(PlayerBehavior* playerBehavior) : playerBehavior_(playerBehavior) {}
  ~PlayerBigShot() {}

  void Init() override;
  void Update(float dt) override;
  void Shutdown() override;

private:
  PlayerBehavior* playerBehavior_;
};

