/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#include "pch.h"
#include "BulletBehavior.h"
#include "Transform.h" 
#include "ObjectManager.h"
#include "Grid.h"
#include "ChildTransform.h"
#include "GameObject.h"
#include "EnvironmentManager.h"

#include "BulletBehavStateDefault.h"

BulletBehavior::BulletBehavior() 
  : shooterID_(-1) {
  // Allocate any memory you need to, but try to avoid calling new


  // Add states using AddStateFunctions
  AddBehaviorState(new BulletBehaveShotState(this));
}

BulletBehavior::~BulletBehavior() {
  // Deallocate memory here
}

void BulletBehavior::Create(int idOfShooter, bool explode, bool damagePlayer) {
  // initialize data here
  shooterID_ = idOfShooter;
  explode_ = explode;
  damagePlayer_ = damagePlayer;

  // Set the initial state using ChangeState
  ChangeState(BBS_SHOT);
  ttl_ = 5;
  hit = false;
}

void BulletBehavior::Update() {
  if (targetID_ < 0)
  {
    return;
  }

  GameObject* thisObject = ObjectManager::GetObjectByID(GetParentID());
  Physics* physics = thisObject->GetComponent<Physics>();
  Transform* transform = thisObject->GetComponent<Transform>();

  float posX = transform->GetPositionX();
  float posY = transform->GetPositionY();
  float posZ = transform->GetPositionZ();

  //std::vector<GameObject*> players = ObjectManager::GetObjectsByName("player");

  Transform* playerTransform = ObjectManager::GetObjectByID(targetID_)->GetComponent<Transform>();
  float playerX = playerTransform->GetPositionX();
  float playerY = playerTransform->GetPositionY();
  float playerZ = playerTransform->GetPositionZ();

  float dispX = playerX - posX;
  float dispY = playerY - posY;
  float dispZ = playerZ - posZ;
  float distance = dispX * dispX + dispZ * dispZ + dispX * dispY;
  if (distance <= 0)
  {
    return;
  }
  distance = sqrt(distance);
  float speed = 0.5f;
  dispX *= speed / distance;
  dispY *= speed / distance;
  dispZ *= speed / distance;

  physics->Teleport(dispX, dispY, dispZ);
}

void BulletBehavior::Destroy() {
  //  Clear memory used, dont deallocaate, this memory might be used by
  //  a similar behavior type later
  shooterID_ = -1;

  Behavior::Shutdown();
}

void BulletBehavior::SetBulletParameters(bool explode, bool damagePlayer)
{
  explode_ = explode;
  damagePlayer_ = damagePlayer;
}

bool BulletBehavior::getExplode()
{
  return explode_;
}

bool BulletBehavior::WillDamagePlayer()
{
  return damagePlayer_;
}

int BulletBehavior::GetShooterID() const {
  return shooterID_;
}

void BulletBehavior::SetTargetID(int tid) {
  targetID_ = tid;
}
