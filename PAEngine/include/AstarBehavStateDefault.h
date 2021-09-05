#pragma once
#include "BehaviorState.h"

class AstarBehavior;

class AstarBehavStateDefault :
  public BehaviorState
{
public:
  AstarBehavStateDefault(AstarBehavior* astarBehavior) : astarBehavior_(astarBehavior) {}
  ~AstarBehavStateDefault() {}

  void Init() override;
  void Update(float dt) override;
  void Shutdown() override;
private:
  AstarBehavior* astarBehavior_;
};

