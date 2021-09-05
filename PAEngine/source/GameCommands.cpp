/**
 * @file GameCommands.cpp
 *
 * @brief It's a implementation of GameCommands.h
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include "GameCommands.h"

#include "serviceLocator.h"
#include "InputHandler.h"

#include "ObjectFactory.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "Behavior.h"
#include "Transform.h" 
#include "PlayerBehavior.h"
#include "PlayerConfig.h"
#include "PlayerController.h"
#include "PlayerAttributesController.h"

#include "AIBehavior.h"

namespace PAEngine
{
  void ForwardFunction(void* data)
  {
    Gamepad* pGamepad = (Gamepad*)data;

    // there are no gamepads, use keyboard and mouse inputs
    if (pGamepad == nullptr)
    {
    }
    // gamepads
    else 
    {
      const float pressPower = pGamepad->GetAnalogZL();
    }

    // get the FIRST player's data
    const int objectid = PlayerController::GetObjectIDFromPlayerID(0);
    Physics* phys = ObjectManager::GetObjectByID(objectid)->GetComponent<Physics>();
    PlayerBehavior* behav = ObjectManager::GetObjectByID(objectid)->GetComponent<PlayerBehavior>();

    behav->moveInputDirectionZ = 1.f;
    //phys->SetImpulse(-behav->GetCurrAimUnitVecX() * PLAYER_MOVEMENT_SPEED, 0.f, -behav->GetCurrAimUnitVecY() * PLAYER_MOVEMENT_SPEED);
  }

  void BackwardFunction(void* data)
  {
    Gamepad* pGamepad = (Gamepad*)data;

    // get the FIRST player's data
    const int objectid = PlayerController::GetObjectIDFromPlayerID(0);
    Physics* phys = ObjectManager::GetObjectByID(objectid)->GetComponent<Physics>();
    PlayerBehavior* behav = ObjectManager::GetObjectByID(objectid)->GetComponent<PlayerBehavior>();

    behav->moveInputDirectionZ = -1.f;
    //phys->SetImpulse(behav->GetCurrAimUnitVecX() * PLAYER_MOVEMENT_SPEED, 0.f, behav->GetCurrAimUnitVecY() * PLAYER_MOVEMENT_SPEED);
  }

  void LeftFunction(void* data)
  {
    Gamepad* pGamepad = (Gamepad*)data;

    // get the FIRST player's data
    const int objectid = PlayerController::GetObjectIDFromPlayerID(0);
    Physics* phys = ObjectManager::GetObjectByID(objectid)->GetComponent<Physics>();
    PlayerBehavior* behav = ObjectManager::GetObjectByID(objectid)->GetComponent<PlayerBehavior>();

    behav->moveInputDirectionX = -1.f;
    //phys->SetImpulse(behav->GetCurrAimUnitVecY() * PLAYER_MOVEMENT_SPEED, 0.f, -behav->GetCurrAimUnitVecX() * PLAYER_MOVEMENT_SPEED);
  }

  void RightFunction(void* data)
  {
    Gamepad* pGamepad = (Gamepad*)data;

    // get the FIRST player's data
    const int objectid = PlayerController::GetObjectIDFromPlayerID(0);
    Physics* phys = ObjectManager::GetObjectByID(objectid)->GetComponent<Physics>();
    PlayerBehavior* behav = ObjectManager::GetObjectByID(objectid)->GetComponent<PlayerBehavior>();

    behav->moveInputDirectionX = 1.f;
    //phys->SetImpulse(-behav->GetCurrAimUnitVecY() * PLAYER_MOVEMENT_SPEED, 0.f, behav->GetCurrAimUnitVecX() * PLAYER_MOVEMENT_SPEED);
  }

  void DashFunction(void* data)
  {
    //Util::ServiceLocator::getFileLogger()->print(Util::SeverityType::info, "Dash!!");
  }

  void FireFunction(void* data)
  {
    Gamepad* pGamepad = (Gamepad*)data;

    std::string s = "Fire!!";

    if (pGamepad != nullptr)
    {
      const float pressPower = pGamepad->GetAnalogZR();
      s = "Player " + std::to_string(pGamepad->GetPlayerID() + 1) + " ";
      s += "Fire!! Trigger Power:" + std::to_string(pressPower);
    }

    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, s);

    int objectid = PlayerController::GetObjectIDFromGamePad(pGamepad);
    PlayerBehavior* behav = ObjectManager::GetObjectByID(objectid)->GetComponent<PlayerBehavior>();
    if (behav) 
    {
      float currentTime = behav->GetTimer();
      float stamp = behav->getTimeStamp();
      if (currentTime > stamp) {
        behav->Shoot(2.5f, 800.f, 300.f);
      }

      else
        behav->setTimeStamp(currentTime + PLAYER_BIG_SHOT_FIRERATE);
    }
	}

  // Minsu's Big Shot
  void PlayerBigShot(void* data)
  {
    Gamepad* pGamepad = (Gamepad*)data;

    int objectid;
    PlayerBehavior* behav;

    // For the player 0 to take a KeyboardMouse input
    if (pGamepad == nullptr)
    {
      objectid = PlayerController::GetObjectIDFromPlayerID(0);
    }
    // Gamepad input
    else
    {
      objectid = PlayerController::GetObjectIDFromGamePad(pGamepad);
    }

    behav = ObjectManager::GetObjectByID(objectid)->GetComponent<PlayerBehavior>();
    if (behav) {
      behav->ChangeState(PlayerBehavior::PlayerBehaviorState::PBS_BIGSHOT);
    }
  }
  void BigShotPlaying(void* data)
  {
    Gamepad* pGamepad = (Gamepad*)data;

    int objectid;
    PlayerBehavior* behav;

    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "In MBS.\n");

    // For the player 0 to take a KeyboardMouse input
    if (pGamepad == nullptr)
    {
      objectid = PlayerController::GetObjectIDFromPlayerID(0);
    }
    // Gamepad input
    else
    {
      objectid = PlayerController::GetObjectIDFromGamePad(pGamepad);
    }
    behav = ObjectManager::GetObjectByID(objectid)->GetComponent<PlayerBehavior>();

    if (behav)
      behav->ChangeState(PlayerBehavior::PlayerBehaviorState::PBS_PLAYING);
  }

  // Ryan's MachineGun
  void PlayerBehaviorRapidFire(void* data)
  {
    Gamepad* pGamepad = (Gamepad*)data;

    int objectid;
    PlayerBehavior* behav;

    // For the player 0 to take a KeyboardMouse input
    if (pGamepad == nullptr)
    {
      objectid = PlayerController::GetObjectIDFromPlayerID(0);
    }
    // Gamepad input
    else
    {
      objectid = PlayerController::GetObjectIDFromGamePad(pGamepad);
    }
    
    behav = ObjectManager::GetObjectByID(objectid)->GetComponent<PlayerBehavior>();
    if (behav) {
      behav->ChangeState(PlayerBehavior::PlayerBehaviorState::PBS_RAPID_FIRE);
    }
  }
  void PlayerBehaviorPlaying(void* data)
  {
    Gamepad* pGamepad = (Gamepad*)data;

    int objectid;
    PlayerBehavior* behav;

    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "In RapidFireFunction.\n");

    // For the player 0 to take a KeyboardMouse input
    if (pGamepad == nullptr)
    {
      objectid = PlayerController::GetObjectIDFromPlayerID(0);
    }
    // Gamepad input
    else
    {
      objectid = PlayerController::GetObjectIDFromGamePad(pGamepad);
    }
    behav = ObjectManager::GetObjectByID(objectid)->GetComponent<PlayerBehavior>();

    if (behav)
      behav->ChangeState(PlayerBehavior::PlayerBehaviorState::PBS_PLAYING);
  }

  // This is just a test function
  void TestFunction(void* data)
  {
    // Player #1, Move Faster!!
    PlayerAttController::SetMoveVelocity(0, 100.f);
    PlayerAttController::SetMoveMaxVelocity(0, 100.f);

    // Turrets, Aim to the player #1
    GameObject* pObjP1 = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(0));
    Transform* pTrans = pObjP1->GetComponent<Transform>();

    for (auto pObjAI : ObjectManager::GetObjectsByName("player_AI"))
    {
      AIBehavior* pAIBehav = pObjAI->GetComponent<AIBehavior>();

      pAIBehav->SetHeadDirection(
        pTrans->GetPositionX(),
        pTrans->GetPositionY(),
        pTrans->GetPositionZ()
      );
    }

    //// Player #1, Dead!
    //PlayerAttController::SetHP(0, 0);
  }

  void KillFunction(void* data)
  {
    // Player #1, Dead!
    PlayerAttController::SetHP(0, 0);
  }
}