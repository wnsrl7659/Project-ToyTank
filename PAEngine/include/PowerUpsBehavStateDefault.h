#pragma once
#include "BehaviorState.h"

class PowerUpsBehavior;

class PowerUpsBehavStateDefault :
  public BehaviorState
{
public:
  PowerUpsBehavStateDefault(PowerUpsBehavior* PowerUpsBehavior) : PowerUpsBehavior_(PowerUpsBehavior) {}
  ~PowerUpsBehavStateDefault() {}

  void Init() override;
  void Update(float dt) override;
  void Shutdown() override;
private:
  PowerUpsBehavior* PowerUpsBehavior_;
};

