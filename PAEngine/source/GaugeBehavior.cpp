/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#include "pch.h"
#include "FlagBehavior.h"
#include "Transform.h" 
#include "ObjectManager.h"
#include "Grid.h"
#include "ChildTransform.h"
#include "EnvironmentManager.h"
#include "Physics.h"
#include "PlayerWin.h"
#include "BulletBehavior.h"
#include "PlayerBehavior.h"
#include "GaugeBehavior.h"
#include "PlayerController.h"

#include "GaugeBehavStateDefault.h"

GaugeBehavior::GaugeBehavior()
  : gamepad_(nullptr), forces_(nullptr) {
  AddBehaviorState(new GaugeBehavStateDefault(this));
  
}

GaugeBehavior::~GaugeBehavior() {
}

void GaugeBehavior::Create(int parentID) {
  ChangeState(FBS_NEUTRAL);
}

void GaugeBehavior::Destroy() {
  gamepad_ = nullptr;
  Behavior::Shutdown();
}

void GaugeBehavior::GaugeUpdate()
{
  
  //  update Gauges movement from gamepad
  GameObject* gauge = ObjectManager::GetObjectByID(GetParent());
  
  Physics* physics = gauge->GetComponent<Physics>();
  
  Transform* transform = gauge->GetComponent<Transform>();
  float posX = transform->GetPositionX();
  float posZ = transform->GetPositionZ();
  BoxCollider* collider = physics->box;

  FlagBehavior* flag = (ObjectManager::GetObjectsByName("flag")[0])->GetComponent<FlagBehavior>();

  GameObject* player = ObjectManager::GetObjectByID(targetObjectID);
  Transform* playerTransform = player->GetComponent<Transform>();
  float playerX = playerTransform->GetPositionX();
  float playerZ = playerTransform->GetPositionZ();

  float dispX = playerX - posX;
  float dispZ = playerZ - posZ;
  
  float scaleX = (initialScaleX * (flag->GetWinTime() - flag->GetTime(ID)) / flag->GetWinTime());

  if (scaleX < 0.1f)
  {
    scaleX = 0.1f;
  }

  collider->setScale(scaleX, initialScaleY, 1);
  
  physics->Teleport(dispX, 0, dispZ);
  
  
}
