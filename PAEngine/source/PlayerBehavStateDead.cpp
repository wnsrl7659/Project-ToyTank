/******************************************************************************
  file: PlayerBehavStateDead.cpp
  author: m.kang
  (c) DigiPen Institute of Technology
******************************************************************************/

#include "pch.h"
#include "PlayerBehavStateDead.h"
#include "playerBehavior.h"
#include "Behavior.h"
#include "Transform.h"
#include "ChildTransform.h"
#include "Physics.h"
#include "GameObject.h"
#include "CameraManager.h"
#include "ObjectManager.h"
#include "ParticleEmitter.h"
#include "TransformManager.h" // Physics Manager
#include "PlayerController.h"
#include "PlayerConfig.h"
#include "FlagBehavior.h"
#include "AIBehavior.h"
#include "ModelManager.h"

#define _USE_MATH_DEFINES
#include <math.h>

void PlayerBehavStateDead::Init() {
    printf("Player #%d (Object ID %d) is in dead state.\n",
        playerBehavior_->GetPlayerID() + 1,
        playerBehavior_->parentID_);

    const int parentID = playerBehavior_->parentID_; // AKA my ID
    CameraComponent* cam = ObjectManager::GetObjectByID(parentID)->GetComponent<CameraComponent>();
    PlayerBehavior* behav = ObjectManager::GetObjectByID(parentID)->GetComponent<PlayerBehavior>();
    Physics* pBodyPhys = ObjectManager::GetObjectByID(parentID)->GetComponent<Physics>();
    FlagBehavior* flagbehav = ObjectManager::GetObjectsByName("flag")[0]->GetComponent<FlagBehavior>();

    // the name is needes to change for collision callback funtions
    ObjectManager::GetObjectByID(parentID)->SetName("dead"); // tag for collision and others
    // enable to be rotated by external force
    pBodyPhys->box->rigidbody->setAngularFactor(btVector3(1.f, 1.f, 1.f));

    cam->StartShake(120, 30, 0.15f);
    behav->StartGamepadVibration(0.5f, 1.f, 1.f);

    GameObject* shooter = ObjectManager::GetObjectByID(parentID);
    int attackerid;
    if (shooter->GetComponent<PlayerBehavior>())
        attackerid = shooter->GetComponent<PlayerBehavior>()->GetPlayerID();
    else
        attackerid = ObjectManager::GetObjectByID(shooter->GetComponent<AIBehavior>()->GetOwner())->GetComponent<PlayerBehavior>()->GetPlayerID();

    flagbehav->ChangeColors(behav->shotBy, attackerid);

    // Discouple All constraints which is connected with the body
    for (int i = 0; i < pBodyPhys->box->rigidbody->getNumConstraintRefs(); ++i)
    {
        btHingeAccumulatedAngleConstraint* pAccuHingeWithBody = reinterpret_cast<btHingeAccumulatedAngleConstraint*>(pBodyPhys->box->rigidbody->getConstraintRef(i));
        pAccuHingeWithBody->setEnabled(false);

        // apply the same amount of gravity to the head. (The gravity for the head was off)
        pAccuHingeWithBody->getRigidBodyB().setGravity(pBodyPhys->box->rigidbody->getGravity());
        // enable to be rotated by external force
        pAccuHingeWithBody->getRigidBodyB().setAngularFactor(btVector3(1.f, 1.f, 1.f));
    }

    // Disable the emitter on "player_body"
    for (auto objChildID : ObjectManager::GetChildObjects(playerBehavior_->partBodyID))
    {
        GameObject* pObjChild = ObjectManager::GetObjectByID(objChildID);
        if (pObjChild->GetName() == "player_body")
        {
            ParticleEmitter* pEmitter = pObjChild->GetComponent<ParticleEmitter>();
            pEmitter->SetActive(false);

            pObjChild->SetName("body_dead");
        }
        else if (pObjChild->GetName() == "player_indicator")
        {
            size_t id = pObjChild->GetComponent<ModelComponent>()->GetInstance();
            ModelManager::GetModel(MT_MONO_COLOR_CUBE)->SetAlpha(id, 0.f);
        }
    }

    // Disable the emitter which is related to FuelDepot. (FuelDepotBehavior::StopFillingPlayer)
    ObjectManager::GetObjectByID(this->playerBehavior_->partBodyIDParticle)->GetComponent<ParticleEmitter>(2)->SetActive(false);

    // Disable the ground coloring
    pBodyPhys->box->clearCollisionTargets();
}

void PlayerBehavStateDead::Update(float dt) {
    // DO NOT update the player behavior! It's dead!!
    //playerBehavior_->playerUpdate(dt);

    GameObject* thisObject = ObjectManager::GetObjectByID(playerBehavior_->GetParentID());

    if (playerBehavior_->gamepad_ != nullptr)
    {
      playerBehavior_->Vibration(dt);
      playerBehavior_->respawnTimer += dt;

      // Respawning!
      if (playerBehavior_->respawnTimer >= PLAYER_RESPAWN_TIME)
      {
        Transform* trans = thisObject->GetComponent<Transform>();

        // the position to be revived
        vec3 revivepos(
          playerBehavior_->initialX - trans->GetPositionX(),
          0,
          playerBehavior_->initialZ - trans->GetPositionZ()
        );

        // create a new tank and assign it to the player
        PlayerController::GetPlayerData(
          playerBehavior_->gamepad_->GetPlayerID())->RecreateObject(
            revivepos.x,
            revivepos.y,
            revivepos.z
          );

        printf("Object ID %d of Player #%d is created.\n",
          playerBehavior_->GetPlayerID(),
          PlayerController::GetObjectIDFromPlayerID(playerBehavior_->gamepad_->GetPlayerID()));

        ///////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////

        TerminateThisState();
      }
    }
    // Due to no gamepad data, the current behavior will not be used anymore!!
    else
      TerminateThisState();
}

void PlayerBehavStateDead::Shutdown() {
}

// This means the current behavior will not be used anymore!!
// So this function should be called in the most bottom of a logic!!
void PlayerBehavStateDead::TerminateThisState()
{
  GameObject* thisObject = ObjectManager::GetObjectByID(playerBehavior_->GetParentID());
  thisObject->GetComponent<Behavior>()->ChangeState(-1);

  playerBehavior_->gamepad_ = nullptr;
  playerBehavior_->forces_ = nullptr;
}