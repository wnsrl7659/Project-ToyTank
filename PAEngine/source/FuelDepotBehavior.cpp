#include "pch.h"
#include "FuelDepotBehavior.h"
#include "GameObject.h"
#include "ObjectManager.h"
#include "ParticleEmitter.h"
#include "PlayerBehavior.h"
#include "Transform.h"
#include "ChildTransform.h"
#include "Physics.h"
#include "FuelDepotBehavStateDefault.h"
#include "ParticleEmitter.h"
#include "PlayerController.h"
#include "EventSystem.h"

FuelDepotBehavior::FuelDepotBehavior()
  : fuelReserves_(0), maxFuel_(0), fillSpeed_(0), reservesFillSpeed_(0), reserverFillTimer_(0), pe_(nullptr), playersInDepot_({ 0 }) {
  AddBehaviorState(new FuelDepotBehavStateDefault(this));
}

FuelDepotBehavior::~FuelDepotBehavior()
{
}

void FuelDepotBehavior::Create(int parentID, int fuelReserves, float playerFillSpeed, float reservesFillspeed) {
    GameObject* gameObject = ObjectManager::GetObjectByID(parentID);
    pe_ = gameObject->GetComponent<ParticleEmitter>();
    this->ChangeState(FDBS_DEFAULT);

    // fill in data
    fuelReserves_ = fuelReserves;
    maxFuel_ = fuelReserves;
    fillSpeed_ = playerFillSpeed;
    reserverFillTimer_ = reservesFillspeed;
    reservesFillSpeed_ = reservesFillspeed;

  //fill in player behaviors pointers
  std::vector<GameObject*>players = ObjectManager::GetObjectsByName("player");
  for (unsigned i = 0; i < players.size(); ++i) {
    playersInDepot_[i] = false;
  }
}

void FuelDepotBehavior::StartFillingPlayer(int playerID) {
    playersInDepot_[playerID] = true;
    GameObject* player = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID));
    PlayerBehavior* playerBehav = player->GetComponent<PlayerBehavior>();
    ParticleEmitter* pe = ObjectManager::GetObjectByID(player->GetComponent<PlayerBehavior>()->partBodyIDParticle)->GetComponent<ParticleEmitter>(2);
    pe->SetColorBehavior(player->GetComponent<PlayerBehavior>()->colorOrigin, player->GetComponent<PlayerBehavior>()->colorOrigin, 1, 0);
    pe->SetActive(true);
}

void FuelDepotBehavior::StopFillingPlayer(int playerID) {
    playersInDepot_[playerID] = false;
    auto players = ObjectManager::GetObjectsByName("player");
    if(playerID < players.size())
    {
      GameObject* player = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID));
      ParticleEmitter* pe = ObjectManager::GetObjectByID(player->GetComponent<PlayerBehavior>()->partBodyIDParticle)->GetComponent<ParticleEmitter>(2);
      pe->SetActive(false);
    }
}

bool FuelDepotBehavior::CheckPlayerInRange(int playerID) const {
    vec3 thisPos, playerPos, distVec;
    Transform* thisTrans = ObjectManager::GetObjectByID(this->parentID_)->GetComponent<Transform>();
    //Transform* playerTrans = ObjectManager::GetObjectByID(playerBehaviors_[playerID]->GetParent())->GetComponent<Transform>();
    Transform* playerTrans = ObjectManager::GetObjectByID(ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID))->GetComponent<PlayerBehavior>()->GetParent())->GetComponent<Transform>();
    thisTrans->GetPosition((float*)&thisPos);
    playerTrans->GetPosition((float*)&playerPos);
    distVec = thisPos - playerPos;
    float squareDist = distVec.getSquareLength();

    return squareDist < 90;
}

bool FuelDepotBehavior::PlayerInDepot(int playerID) const
{
    return playersInDepot_[playerID];
}

void FuelDepotBehavior::FillPlayer(int playerID) {
    float& timer = this->GetTimer();
    if (fuelReserves_ > 0 && timer > fillSpeed_) {
        //playerBehaviors_[playerID]->fuel += 1;
        PlayerBehavior* pPlayerBehavior = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(playerID))->GetComponent<PlayerBehavior>();
        const bool isDead = !(ObjectManager::GetObjectByID(pPlayerBehavior->GetParent())->GetName().compare("dead"));
        
        if (isDead == false && pPlayerBehavior->fuel < 100 /*pPlayerBehavior->maxFuel */) {
          pPlayerBehavior->fuel += 1;
          fuelReserves_ -= 1;
          timer = 0;
          EventSystem::SendEvent(ET_SOUND_OS_REFUEL, &(pPlayerBehavior->fuel), sizeof(float));
            if (pPlayerBehavior->fuel == 100 /*pPlayerBehavior->maxFuel */) {
                EventSystem::SendEvent(ET_SOUND_OS_FUELFULL, nullptr, 0);
            }
        }
    }
}
