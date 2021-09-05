#include "pch.h"
#include "FuelDepotBehavStateDefault.h"
#include "FuelDepotBehavior.h"
#include "GameObject.h"
#include "ObjectManager.h"
#include "PlayerController.h"
#include "PlayerBehavior.h"

FuelDepotBehavStateDefault::FuelDepotBehavStateDefault(FuelDepotBehavior* depot)
  : depotBehavior_(depot) {}

FuelDepotBehavStateDefault::~FuelDepotBehavStateDefault() {
}

void FuelDepotBehavStateDefault::Init() {
}


void FuelDepotBehavStateDefault::Update(float dt) {
  // refill the fuel reserves
  // refilling player fuel will be handled in collision handler
  if (depotBehavior_->fuelReserves_ < depotBehavior_->maxFuel_) {
    if (depotBehavior_->reserverFillTimer_ > depotBehavior_->reservesFillSpeed_) {
      depotBehavior_->fuelReserves_++;
      depotBehavior_->reserverFillTimer_ = 0.f;
    }
  }
  else {
    depotBehavior_->reserverFillTimer_ += dt;
  }

  std::vector<GameObject*>players = ObjectManager::GetObjectsByName("player");
  if (players.size() == 0)
    return;

  // fill players in the depot
  for (unsigned i = 0; i < depotBehavior_->playersInDepot_.size(); ++i) {
    //if (depotBehavior_->playerBehaviors_[i]) {
    GameObject* pPlayer = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(i));
    if (pPlayer == nullptr)
      return;

    PlayerBehavior* pPlayerBehavior = pPlayer->GetComponent<PlayerBehavior>();
    if(pPlayerBehavior) {
      // check if players left the depot
      if (depotBehavior_->CheckPlayerInRange(i)) {
        depotBehavior_->StartFillingPlayer(i);
      }
      else {
        depotBehavior_->StopFillingPlayer(i);
      }
      if (depotBehavior_->PlayerInDepot(i)) {
        depotBehavior_->FillPlayer(i);

      }
    }
  }

}

void FuelDepotBehavStateDefault::Shutdown() {
}
