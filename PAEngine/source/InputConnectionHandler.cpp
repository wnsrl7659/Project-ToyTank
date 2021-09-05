/**
 * @file InputConnectionHandler.cpp
 *
 * @brief It manages connection and disconnection issues
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include "InputConnectionHandler.h"
#include "InputHandler.h"
#include "ServiceLocator.h"

#include "PlayerController.h"
#include "CameraManager.h"
#include "ObjectManager.h"
#include "PlayerBehavior.h"
#include "PlayerAttributesController.h"

namespace PAEngine
{
  void DeviceConnected(Gamepad* const pGamepad)
  {
    const int playerID = pGamepad->GetPlayerID();

    PlayerController::PairPlayerToGamepad(playerID, pGamepad);
    PlayerController::GetPlayerData(playerID)->Init(playerID);

    std::string s;
    s = "P" + std::to_string(pGamepad->GetPlayerID() + 1);
    s += ", All components for the player have been initialized.";
    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, s);
  }

  void DeviceDisconnected(Gamepad* const pGamepad)
  {
    const int playerID = pGamepad->GetPlayerID();
    const int objectID = PlayerController::GetObjectIDFromGamePad(pGamepad);
    GameObject* pObject = ObjectManager::GetObjectByID(objectID);
    PlayerBehavior* pBehav = pObject->GetComponent<PlayerBehavior>();

    PlayerAttController::SetHP(playerID, 0);                  // kill the player's tank
    PlayerController::PairPlayerToGamepad(playerID, nullptr); // clear the player's data in PlayerControler
    GetCameraManager()->removeCamera(playerID);               // Remove the camera which have been attached to the player
    pBehav->ClearGamepad();

    std::string s;
    s = "P" + std::to_string(pGamepad->GetPlayerID() + 1);
    s += ", All components for the player have been deleted.";
    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, s);
  }
}