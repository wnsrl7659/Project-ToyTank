#pragma once
#include "BehaviorState.h"
class AIBehavior;

class AIBehavStateDefault :
  public BehaviorState
{
public:
  AIBehavStateDefault(AIBehavior* aiBehavior) : aiBehavior_(aiBehavior) {}
  ~AIBehavStateDefault() {}

  void Init() override;
  void Update(float dt) override;
  void Shutdown() override;

private:
  AIBehavior* aiBehavior_;
};

