/******************************************************************************
  file: PlayerBehavStateDefault.cpp
  author: j.mcdonell
  (c) DigiPen Institute of Technology
******************************************************************************/

#include "pch.h"
#include "PlayerBehavStateDefault.h"
#include "playerBehavior.h"
#include "Behavior.h"
#include "Transform.h"
#include "ChildTransform.h"
#include "Physics.h"
#include "GameObject.h"
#include "CameraManager.h"
#include "ObjectManager.h"

#define _USE_MATH_DEFINES
#include <math.h>

void PlayerBehavStateDefault::Init() {
  GameObject* thisObject = ObjectManager::GetObjectByID(playerBehavior_->GetParentID());


  Transform* transform = thisObject->GetComponent<Transform>();
  playerBehavior_->initialX = transform->GetPositionX();
  playerBehavior_->initialZ = transform->GetPositionZ();
}

void PlayerBehavStateDefault::Update(float dt) {

  playerBehavior_->playerUpdate(dt);
  playerBehavior_->Vibration(dt);
}

void PlayerBehavStateDefault::Shutdown() {
}
