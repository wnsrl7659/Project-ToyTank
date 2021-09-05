#pragma once
#include "BehaviorState.h"
class BulletBehavior;

class BulletBehaveShotState : public BehaviorState
{
public:
  BulletBehaveShotState(BulletBehavior* bulletBehavior) : bulletBehavior_(bulletBehavior) {}
  ~BulletBehaveShotState() {}

  void Init() override;
  void Update(float dt) override;
  void Shutdown() override;
  void DestroyLater();

private:
  BulletBehavior* bulletBehavior_;
  const float startTime = 5.0f;
  //float ttl_ = startTime;
};

