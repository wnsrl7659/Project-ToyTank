#pragma once
#include "BehaviorState.h"

class FuelDepotBehavior;
class FuelDepotBehavStateDefault :
  public BehaviorState
{
public:
  FuelDepotBehavStateDefault(FuelDepotBehavior* depot);
  ~FuelDepotBehavStateDefault();
  void Init() override;
  void Update(float dt) override;
  void Shutdown() override;

private:
  FuelDepotBehavior* depotBehavior_;
};

