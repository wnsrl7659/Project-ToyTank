/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#include "pch.h"
#include "PowerUpsBehavior.h"
#include "Transform.h" 
#include "ObjectManager.h"

#include "ChildTransform.h"
#include "EnvironmentManager.h"
#include "Physics.h"
#include "PlayerWin.h"
#include "BulletBehavior.h"
#include "PlayerBehavior.h"
#include "Grid.h"
#include "GameObject.h"
#include "ObjectFactory.h"

#include "PowerUpsBehavStateDefault.h"
#include "PlayerAttributesController.h"

#include "ModelManager.h"
#include <random>
#include <chrono>
#include <ctime>
#include <cmath>

void PlayerCollidesWithPowerUps_CB(unsigned myID, unsigned otherID, const std::string& bullet)
{
    PowerUpsBehavior* behav = ObjectManager::GetObjectByID(myID)->GetComponent<PowerUpsBehavior>();
    GameObject* player = ObjectManager::GetObjectByID(otherID);

    // get the core player object instead of the head object or other player attached object
    while (player->GetParent() != -1)
    {
        otherID = player->GetParent();
        player = ObjectManager::GetObjectByID(otherID);
    }

    PlayerBehavior* playerbehav = player->GetComponent<PlayerBehavior>();
    playerbehav->SetPowerUpTimer();
    // Reset Player
    PlayerAttController::ResetAllAttributes(playerbehav->GetPlayerID());
    int type = behav->powerUpType;
    //type = 1;
    switch (type)
    {
    case 0:
        playerbehav->RecoverHP();
        break;
    case 4:
        PlayerAttController::SetFireRate(playerbehav->GetPlayerID(), 0.08f);
        playerbehav->SetPowerUpTimer();
        break;
    case 2:
        playerbehav->invincible();
        playerbehav->SetPowerUpTimer();
        break;
    case 3:
        PlayerAttController::SetMoveMaxVelocity(playerbehav->GetPlayerID(), 50);
        PlayerAttController::SetMoveVelocity(playerbehav->GetPlayerID(), 50);
        playerbehav->SetPowerUpTimer();
        playerbehav->fast = true;
        break;
    case 1:
        PlayerAttController::SetMoveMaxVelocity(playerbehav->GetPlayerID(), 10);
        PlayerAttController::SetMoveVelocity(playerbehav->GetPlayerID(), 15);
        playerbehav->SetPowerUpTimer();
        playerbehav->slow = true;
        break;

    default:
        break;
    }
    // Destroy
    ObjectManager::DestroyObject(myID);
}
PowerUpsBehavior::PowerUpsBehavior()
    : gamepad_(nullptr), forces_(nullptr) {
    AddBehaviorState(new PowerUpsBehavStateDefault(this));
}

PowerUpsBehavior::~PowerUpsBehavior() {
}

void PowerUpsBehavior::Create(int parentID) {
    ChangeState(PBS_NEUTRAL);

    GameObject* gameObj = ObjectManager::GetObjectByID(parentID_);
    gameObj->GetComponent<Physics>()->box->addCollisionCallback((OnCollision*)&PlayerCollidesWithPowerUps_CB, "player");

    std::mt19937 gen;
    gen.seed(static_cast<unsigned>(std::time(0)));
    std::uniform_real_distribution<float> dis(0, 5);
    powerUpType = unsigned(dis(gen));
    if (powerUpType > 4)
        powerUpType = 4;

    ModelComponent* model = gameObj->GetComponent<ModelComponent>();
    size_t id = model->GetInstance();
    ModelManager::GetModel(model->GetModeltype())->SetColor(id, 0, 0, 1);
    //if (powerUpType >= 4)
    //{
        //m->SetColor(id, 1, 0, 0);
    //}
    //else   if (powerUpType == 3)
    //{
        //m->SetColor(id, 0, 1, 0);
    //}
    //else   if (powerUpType == 2)
    //{
        //m->SetColor(id, 1, 0, 1);
    //}
    //else   if (powerUpType == 1)
    //{
    //m->SetColor(id, 0, 0, 1);
    //}
    //else
    //{
        //m->SetColor(id, 1, 1, 0);
    //}
}

void PowerUpsBehavior::Update(float dt)
{
    PowerUpsUpdate();
}

void PowerUpsBehavior::Destroy() {
    gamepad_ = nullptr;
    Behavior::Shutdown();
}


void PowerUpsBehavior::PowerUpsUpdate()
{
    if (updateDelayCount > updateDelay)
    {
        GameObject* PU = ObjectManager::GetObjectByID(GetParent());

        Physics* physics = PU->GetComponent<Physics>();
        physics->box->rigidbody->setAngularVelocity(btVector3(0.f, 1.f, 0.f));
        //physics->box->rigidbody->applyTorque(btVector3(0.f, 1.f, 0.f));
        physics->box->rigidbody->setLinearVelocity(btVector3(0.f, 0.f, 0.f));

        Transform* transform = PU->GetComponent<Transform>();
        float posX = transform->GetPositionX();
        float posY = transform->GetPositionY();
        float posZ = transform->GetPositionZ();

        auto start = std::chrono::system_clock::now();

        physics->box->setPosition({ posX, (float(std::cos(GetTimer())) / 1.5f) - 0.5f, posZ });
        ++powerUpType;
        powerUpType %= 4;

        //powerUpType = 1;
    }
    ++updateDelayCount;
}
