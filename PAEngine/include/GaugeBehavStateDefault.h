#pragma once
#include "BehaviorState.h"

class GaugeBehavior;

class GaugeBehavStateDefault:
  public BehaviorState
{
public:
  GaugeBehavStateDefault(GaugeBehavior* gaugeBehavior) : gaugeBehavior_(gaugeBehavior) {}
  ~GaugeBehavStateDefault() {}

  void Init() override;
  void Update(float dt) override;
  void Shutdown() override;

private:
  GaugeBehavior* gaugeBehavior_;
};

