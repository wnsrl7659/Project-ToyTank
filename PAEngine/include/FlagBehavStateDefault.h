#pragma once
#include "BehaviorState.h"

class FlagBehavior;

class FlagBehavStateDefault :
  public BehaviorState
{
public:
  FlagBehavStateDefault(FlagBehavior* flagBehavior) : flagBehavior_(flagBehavior) {}
  ~FlagBehavStateDefault() {}

  void Init() override;
  void Update(float dt) override;
  void Shutdown() override;
private:
  FlagBehavior* flagBehavior_;
};

