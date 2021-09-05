#include "pch.h"
#include "BulletBehavStateDefault.h"
#include "BulletBehavior.h"
#include "ObjectManager.h"

void BulletBehaveShotState::Init()
{
  //ttl_ = startTime;
}

void BulletBehaveShotState::Update(float dt)
{
  // delete bullet when ttl is up
  bulletBehavior_->ttl_ -= dt;
  if (bulletBehavior_->ttl_ <= 0.0f)
  {
    ObjectManager::GetInstance()->DestroyObject(bulletBehavior_->GetParentID());
  }
  bulletBehavior_->Update();
}

void BulletBehaveShotState::Shutdown()
{

}

void BulletBehaveShotState::DestroyLater()
{
  //bulletBehavior_->ttl_ = 0;
}
