/**
 * @file PlayerAttributesController.cpp
 *
 * @brief It controlls player's attributes
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include "PlayerAttributesController.h"

#include "PlayerController.h"
#include "PlayerConfig.h"
#include "PlayerBehavior.h"
#include "ObjectManager.h"

#include "ModelManager.h"
#include "ModelComponent.h"

// PlayerID: 0 ~ 3
// reset to the origin attributes. (the data comes from PlayerConfig.h)
void PlayerAttController::ResetAllAttributes(const int playerID)
{
  SetFireRate(playerID, PLAYER_WEAPONE_FIRERATE);
  SetMoveVelocity(playerID, PLAYER_MOVEMENT_SPEED);
  SetMoveMaxVelocity(playerID, PLAYER_MOVEMENT_MAX_SPEED);
}

///////////////////////////////////////////////////////////////////////////////
// Setters ////////////////////////////////////////////////////////////////////

void PlayerAttController::SetHP(const int playerID, const int hp)
{
  GameObject* pObj = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID));
  PlayerBehavior* pBehav = pObj->GetComponent<PlayerBehavior>();

  pBehav->HP = hp;
}

// PlayerID: 0 ~ 3, fire rate is sec.
void PlayerAttController::SetFireRate(const int playerID, const float fireRate)
{
  GameObject* pObj = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID));
  PlayerBehavior* pBehav = pObj->GetComponent<PlayerBehavior>();

  pBehav->weaponeFirerate = fireRate;
}

// PlayerID: 0 ~ 3, velocity is the power of impulse
void PlayerAttController::SetMoveVelocity(const int playerID, const float moveVelocity)
{
  GameObject* pObj = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID));
  PlayerBehavior* pBehav = pObj->GetComponent<PlayerBehavior>();

  pBehav->moveVelocity = moveVelocity;
}

void PlayerAttController::SetMoveMaxVelocity(const int playerID, const float maxVelocity)
{
  GameObject* pObj = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID));
  PlayerBehavior* pBehav = pObj->GetComponent<PlayerBehavior>();

  pBehav->SetMaxVelocity(maxVelocity);
}

void PlayerAttController::SetTwinkling(const int playerID, const int isTwinkling)
{
  GameObject* pObj = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID));
  PlayerBehavior* pBehav = pObj->GetComponent<PlayerBehavior>();

  pBehav->SetTwinkling(isTwinkling);
}

void PlayerAttController::SetColor(const int playerID, const vec3 color)
{
  // set to the origin color
  GameObject* thisObject = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID));
  PlayerBehavior* pBehav = thisObject->GetComponent<PlayerBehavior>();
  
  pBehav->colorOrigin = color;
  GameObject* head = nullptr;

  ModelComponent* pModel;
  for (auto childObj : thisObject->GetChildren())
  {
    GameObject* partObj = ObjectManager::GetObjectByID(childObj);

    // Coloring CHILD CHILD objects
    for (auto childChildObj : partObj->GetChildren())
    {
      pModel = ObjectManager::GetObjectByID(childChildObj)->GetComponent<ModelComponent>();
      if (pModel != nullptr)
      {
        ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
          pBehav->colorOrigin.x,
          pBehav->colorOrigin.y,
          pBehav->colorOrigin.z);
      }
    }

    // Coloring child objects
    if (partObj->GetName() == "player_body_Front_1" || partObj->GetName() == "player_body_Front_2" || partObj->GetName() == "player_body_Back" || partObj->GetName() == "player_body")
    {
      continue;
    }

    if (partObj->GetName() == "player_head")
    {
      head = partObj;
      continue;
    }

    pModel = ObjectManager::GetObjectByID(childObj)->GetComponent<ModelComponent>();
    if (pModel != nullptr)
    {
      ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
        pBehav->colorOrigin.x,
        pBehav->colorOrigin.y,
        pBehav->colorOrigin.z);
    }
  }

  if(head != nullptr)
  for (auto childObj : head->GetChildren())
  {
      GameObject* partObj = ObjectManager::GetObjectByID(childObj);
      ModelComponent* pModel = ObjectManager::GetObjectByID(childObj)->GetComponent<ModelComponent>();
      if (pModel != nullptr)
      {
          ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
              pBehav->colorOrigin.x,
              pBehav->colorOrigin.y,
              pBehav->colorOrigin.z);
      }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Getters ////////////////////////////////////////////////////////////////////

// PlayerID: 0 ~ 3, fire rate is sec.
const float PlayerAttController::GetFireRate(const int playerID)
{
  GameObject* pObj = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID));
  PlayerBehavior* pBehav = pObj->GetComponent<PlayerBehavior>();

  return pBehav->weaponeFirerate;
}

// PlayerID: 0 ~ 3, velocity is the power of impulse
const float PlayerAttController::GetMoveVelocity(const int playerID)
{
  GameObject* pObj = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID));
  PlayerBehavior* pBehav = pObj->GetComponent<PlayerBehavior>();

  return pBehav->moveVelocity;
}

const float PlayerAttController::GetMoveMaxVelocity(const int playerID)
{
  GameObject* pObj = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID));
  PlayerBehavior* pBehav = pObj->GetComponent<PlayerBehavior>();

  return pBehav->GetMaxVelocity();
}