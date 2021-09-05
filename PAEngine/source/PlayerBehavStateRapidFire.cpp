/******************************************************************************
  file: PlayerBehavStateRapidFire.cpp
  author: j.mcdonell
  (c) DigiPen Institute of Technology
******************************************************************************/

#include "pch.h"
#include "PlayerBehavStateRapidFire.h"
#include "PlayerBehavior.h"

#include "GameObject.h"
#include "ChildTransform.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "ObjectFactory.h"
#include "ObjectManager.h"
#include "ParticleEmitter.h"


/////***** machinegun shot *****/////

void PlayerBehavStateRapidFire::Init() {
  playerBehavior_->time_ = playerBehavior_->GetTimer();
  GameObject* thisObject = ObjectManager::GetObjectByID(playerBehavior_->GetParentID());

  CameraComponent* cam = thisObject->GetComponent<CameraComponent>();
  //cam->StartShake(120, 30, 0.75);
  ParticleEmitter* pe = ObjectManager::GetObjectByID(playerBehavior_->partHeadID)->GetComponent<ParticleEmitter>();
  pe->SetActive(true);
}

void PlayerBehavStateRapidFire::Update(float dt) { 
  float& timer = playerBehavior_->GetTimer();
  // rapid fire
  if (timer >= playerBehavior_->weaponeFirerate)
  {
    playerBehavior_->Shoot(0.8f, 500.0f, false, true); // scale speed

    timer = 0.0f;

  }

  playerBehavior_->playerUpdate(dt);
}

void PlayerBehavStateRapidFire::Shutdown() {
  ParticleEmitter* pe = ObjectManager::GetObjectByID(playerBehavior_->partHeadID)->GetComponent<ParticleEmitter>();
  pe->SetActive(false);
}


/////***** Big shot *****/////
void PlayerBigShot::Init() 
{
  //EventSystem::SendEvent(ET_SOUND_L_START_RAPIDFIRE, nullptr, 0);
  playerBehavior_->time_ = playerBehavior_->GetTimer();
  GameObject* thisObject = ObjectManager::GetObjectByID(playerBehavior_->GetParentID());

  CameraComponent* cam = thisObject->GetComponent<CameraComponent>();
  ParticleEmitter* pe = ObjectManager::GetObjectByID(playerBehavior_->partHeadID)->GetComponent<ParticleEmitter>();
  pe->SetActive(true);
}

void PlayerBigShot::Update(float dt) {
  float& timer = playerBehavior_->GetTimer();
  // rapid fire
  if (timer >= playerBehavior_->bigShotFireRate)
  {
    playerBehavior_->Shoot(1.0f, 500.0f, true, false); // scale speed

    timer = 0.0f;

  }

  playerBehavior_->playerUpdate(dt);
}

void PlayerBigShot::Shutdown() {
  // EventSystem::SendEvent(ET_SOUND_L_END_RAPIDFIRE, nullptr, 0);
  ParticleEmitter* pe = ObjectManager::GetObjectByID(playerBehavior_->partHeadID)->GetComponent<ParticleEmitter>();
  pe->SetActive(false);
}
