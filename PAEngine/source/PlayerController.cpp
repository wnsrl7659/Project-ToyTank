/******************************************************************************
  file: PlayerController.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "PlayerController.h"
#include "EventSystem.h"
#include "InputDevices.h"
#include "ServiceLocator.h"

std::array<Player, MAX_PLAYERS> PlayerController::players_;

static void HandleGamePadInput(PAEngine::Gamepad* gamepad) {
  //  add input events to the forwarding queue for the right player
}

PlayerController::PlayerController(){
}

PlayerController::~PlayerController() {
}

void PlayerController::Init() {
  PairPlayersToGamepads();
  PairRemainingPlayersToAI();

  players_[0].inputType_ = static_cast<PlayerInputType>(0);
  for (int i = 0; i < players_.size(); ++i)
    players_[i].Init(i);

}

void PlayerController::Update(float dt) {
}

void PlayerController::Shutdown() {
  for (unsigned i = 0; i < players_.size(); ++i) {
    players_[i].Shutdown();
  }
}

void PlayerController::PairPlayerToGamepad(unsigned playerID, PAEngine::Gamepad* gamepad) {
  players_[playerID].gamepad_ = gamepad;
  players_[playerID].inputType_ = static_cast<PlayerInputType>(playerID);

  // if someone wants to delete the data... like hendling a gamepad disconnection.
  if (gamepad == nullptr)
  {
    players_[playerID].objectID_ = -1;
    players_[playerID].behav_ = nullptr;
  }
}

void PlayerController::PairPlayersToGamepads() {
  for (int i = 0; i < XUSER_MAX_COUNT; ++i) {
    PAEngine::Gamepad* pGamepad = Util::ServiceLocator::getGameInput()->GetGamepad(i);
    if (pGamepad != nullptr) {
      players_[i].inputType_ = static_cast<PlayerInputType>(i);
      players_[i].gamepad_ = pGamepad;
    }
  }
}

void PlayerController::PairRemainingPlayersToAI() {
  for (int i = 0; i < players_.size(); ++i) {
    if (players_[i].inputType_ == PIT_INVALID) {
      players_[i].inputType_ = PIT_AI_0;
    }
  }
}

void PlayerController::PairPlayerToAI(unsigned playerID) {
  players_[playerID].inputType_ = PIT_AI_0;
}

int PlayerController::GetObjectIDFromPlayerID(const unsigned playerID)
{
  return players_[playerID].objectID_;
}

int PlayerController::GetObjectIDFromGamePad(PAEngine::Gamepad* gamepad)
{
  return players_[gamepad->GetPlayerID()].objectID_;
}

vec3 PlayerController::GetObjectInitPosFromPlayerID(const unsigned playerID)
{
  return players_[playerID].InitPos_;
}