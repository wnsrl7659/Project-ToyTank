/******************************************************************************
  file: ObjectFactory.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "ObjectFactory.h"
#include "ObjectManager.h"
#include "Transform.h"
#include "ChildTransform.h"
#include "Behavior.h"
#include "BehaviorManager.h"
#include "ChildTransformManager.h"
#include "TransformManager.h"
#include "ParticleEmitterManager.h"
#include "ModelManager.h"
#include "PlayerBehavior.h"
#include "PlayerConfig.h"
#include "InputDevices.h"
#include "AIBehavior.h"
#include "CameraManager.h"
#include "GameObject.h"
#include "FlagBehavior.h"
#include "Physics.h"
#include "BulletBehavior.h"
#include "GaugeBehavior.h"
#include "ParticleEmitter.h"
#include "AstarBehavior.h"
#include "PowerUpsBehavior.h"
#include "FuelDepotBehavior.h"
#include "PAGuiWidgets.h"
#include "PAGuiWindow.h"
#include "BulletBehavStateDefault.h"

#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

template <typename BEHAV_TYPE>
BEHAV_TYPE* AllocateBehavior(BehaviorType type, int parentID) {
    Behavior* behav = nullptr;
    int idx = BehaviorManager::GetInstance()->Allocate();
    behav = BehaviorManager::ReallocateBehaviorType(idx, type, parentID);

    return dynamic_cast<BEHAV_TYPE*>(behav);
}

// a bullet has collided with the ground - do an explosion
void BulletCollidesWithAnything_CB(unsigned myID, unsigned otherID, const std::string& target)
{
    GameObject* go = ObjectManager::GetInstance()->GetObjectByID(otherID);
    GameObject* bullet = ObjectManager::GetInstance()->GetObjectByID(myID);
    if (!go || !bullet)
    {
        printf("BulletCollidesWithAnything_CB failed\n");
        return;
    }
    //printf("::: Collision with %s\n", go->GetName().c_str());

    // dont explode other bullets
    if (go->GetName() == "bullet" || go->GetName().find("player") != std::string::npos)
        return;

    if (bullet->GetComponent<BulletBehavior>()->getExplode())
        bullet->GetComponent<Physics>()->box->explode(5, 8);
}

int ObjectFactory::CreateGround(vec3 position, vec3 size, int index) {
    int n = ObjectManager::GetInstance()->Allocate();
    GameObject* ground = ObjectManager::GetObjectByID(n);
    if (ground) {
        char temp[10];
        itoa(n, temp, 10);
        tn_object->AddWidget(new PAGuiTransform(temp, n));
        ground->SetName("ground");
        Transform* groundTransform = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        Physics* groundPhysics = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));;
        ModelComponent* groundModel = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));
        Behavior* behav = AllocateBehavior<AstarBehavior>(BT_ASTAR, ground->GetID());
        ParticleEmitter* particleCaptured = dynamic_cast<ParticleEmitter*>(ParticleEmitterManager::GetInstance()->Get(ParticleEmitterManager::GetInstance()->Allocate()));
        ParticleEmitter* particleCaptured1 = dynamic_cast<ParticleEmitter*>(ParticleEmitterManager::GetInstance()->Get(ParticleEmitterManager::GetInstance()->Allocate()));

        ground->AddComponent(groundTransform);
        ground->AddComponent(groundPhysics);
        ground->AddComponent(groundModel);
        ground->AddComponent(behav);
        ground->AddComponent(particleCaptured);
        ground->AddComponent(particleCaptured1);

        groundPhysics->Create(ground->GetID(), size.x, size.y, size.z, 0.f, position.x, position.y, position.z);
        groundPhysics->box->rigidbody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
        groundPhysics->box->rigidbody->setActivationState(DISABLE_SIMULATION);

        groundTransform->Create(ground->GetID(), 1, 1, 1, 0, 0, 0);
        groundModel->Create(ground->GetID(), MT_FLOOR_CONCRETE);

        particleCaptured->Create(ground->GetID(), MT_CUBE_METALGRID, 0.0000001f, 0.000001f, 1.f, false);
        particleCaptured->SetColorBehavior({ 1, 1, 1 }, { 1, 1, 1 }, 1, 0);
        particleCaptured->SetTransformBehavior({ 0, 0.1f, 0 }, .5f, { 0.9f, 0, 0, 0, 0, 0.9f, 0, 0, 0, 0, 0.9f, 0, 0, 0, 0, 1 });
        particleCaptured->SetParticleTransform({ 0.6f, 0, 0, 0, 0, .6f, 0, 0, 0, 0, .6f, 0, 2, 0, 2, 1 });

        particleCaptured1->Create(ground->GetID(), MT_CUBE_METALGRID, 0.0000001f, 0.000001f, 1.f, false);
        particleCaptured1->SetColorBehavior({ 1, 1, 1 }, { 1, 1, 1 }, 1, 0);
        particleCaptured1->SetTransformBehavior({ 0, 0.1f, 0 }, .5f, { 0.9f, 0, 0, 0, 0, 0.9f, 0, 0, 0, 0, 0.9f, 0, 0, 0, 0, 1 });
        particleCaptured1->SetParticleTransform({ 0.6f, 0, 0, 0, 0, .6f, 0, 0, 0, 0, .6f, 0, -2, 0, -2, 1 });

        behav = dynamic_cast<AstarBehavior*>(behav->Init(ground->GetID(), BT_ASTAR)); //  initializes the base class, 
        dynamic_cast<AstarBehavior*>(behav)->Create(ground->GetID());
        dynamic_cast<AstarBehavior*>(behav)->index = index;

        return ground->GetID();
    }
    else {
        return -1;
    }
}

int ObjectFactory::CreatePlayer(PAEngine::Gamepad* gamepad, float xPos, float yPos, float zPos) {

    Transform* trans = nullptr;
    Behavior* behav = nullptr;
    ModelComponent* model = nullptr;
    Physics* physics = nullptr;

    Physics* pHeadphys = nullptr;
    Physics* pBodyphys = nullptr;
    ParticleEmitter* pe = nullptr;
    ParticleEmitter* bulletParticles = nullptr;
    ParticleEmitter* particleRecover = nullptr;
    ParticleEmitter* particleRecoverFuel = nullptr;

    btVector3 pivotA = btVector3(0.f, 0.f, 0.f);
    btVector3 pivotB = btVector3(0.f, 0.f, 0.f);
    btHingeAccumulatedAngleConstraint* pHinge = nullptr;

    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    ////////////////
    // Player's Body
    int n = ObjectManager::GetInstance()->Allocate();
    GameObject* playerBody = ObjectManager::GetObjectByID(n);
    playerBody->SetName("player_body");

    GameObject* playerBodyFront1 = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    playerBodyFront1->SetName("player_body_Front_1");
    GameObject* playerBodyFront2 = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    playerBodyFront2->SetName("player_body_Front_2");
    GameObject* playerBodyBack = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    playerBodyBack->SetName("player_body_Back");
    GameObject* playerAccentLeft = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    playerAccentLeft->SetName("player_accent_left");
    GameObject* playerAccentRight = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    playerAccentRight->SetName("player_accent_right");
    GameObject* playerAccentBack = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    playerAccentBack->SetName("player_accent_back");

    GameObject* playerIndicator = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    playerIndicator->SetName("player_indicator");

    GameObject* playerGunAccentBack = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    playerGunAccentBack->SetName("player_gun_accent_back");
    GameObject* playerGunAccentBack2 = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    playerGunAccentBack2->SetName("player_gun_accent_back2");
    GameObject* playerGunAccentBack3 = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    playerGunAccentBack3->SetName("player_gun_accent_back3");

    /////////////////////////
    // Player Object + Camera
    GameObject* player = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    if (player)
    {
        player->SetName("player");
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        pBodyphys = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));;
        behav = AllocateBehavior<PlayerBehavior>(BT_PLAYER, player->GetID());

        // Add Components
        player->AddComponent(trans);
        player->AddComponent(pBodyphys);
        player->AddComponent(behav);

        // Physics
        trans->Create(player->GetID(), 1, 1, 1, 0, 0, 0);
        pBodyphys->Create(player->GetID(), 1.f, 1.f, 1.f,
            15.f,
            xPos, yPos, zPos);
        pBodyphys->box->rigidbody->setAngularFactor(btVector3(0, 1, 0));
        pBodyphys->box->rigidbody->setFriction(PLAYER_MOVEMENT_FRICTION);
        pBodyphys->box->rigidbody->setAngularFactor(btVector3(0.f, 0.f, 0.f)); // it fixes the body's rotating
        pBodyphys->SetMaxVelocity(PLAYER_MOVEMENT_MAX_SPEED);

        // Behavior
        behav = dynamic_cast<PlayerBehavior*>(behav->Init(player->GetID(), BT_PLAYER));
        PlayerBehavior* pPlayerBehav = dynamic_cast<PlayerBehavior*>(behav);
        pPlayerBehav->Create(gamepad, player->GetID());

        // Camera
        if (!GetCameraManager()->isActivated(gamepad->GetPlayerID()))
            GetCameraManager()->addCamera(gamepad->GetPlayerID(), vec3(0.f));

        GetCameraManager()->attachToObject(
            player->GetID(), gamepad->GetPlayerID(),
            vec3(0.f, 3.f, -7.5f),
            vec3(0.f, 0.f, 0.f));
    }

    if (playerBody)
    {
        char temp[10];
        itoa(n, temp, 10);
        tn_object->AddWidget(new PAGuiTransform(temp, n));
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));
        pe = dynamic_cast<ParticleEmitter*>(ParticleEmitterManager::GetInstance()->Get(ParticleEmitterManager::GetInstance()->Allocate()));
        particleRecover = dynamic_cast<ParticleEmitter*>(ParticleEmitterManager::GetInstance()->Get(ParticleEmitterManager::GetInstance()->Allocate()));
        particleRecoverFuel = dynamic_cast<ParticleEmitter*>(ParticleEmitterManager::GetInstance()->Get(ParticleEmitterManager::GetInstance()->Allocate()));

        // Add Components
        playerBody->AddComponent(trans);
        playerBody->AddComponent(model);
        playerBody->AddComponent(pe);
        playerBody->AddComponent(particleRecover);
        playerBody->AddComponent(particleRecoverFuel);

        // Initialize Components
        trans->Create(playerBody->GetID(), 0.75, 0.6f, 1, 0, 0.1, 0);
        model->Create(playerBody->GetID(), MT_CUBE_METALGRID);
        pe->Create(playerBody->GetID(), MT_FLAG, 0.2f, 0.2f, 1.0f, false);
        //particleRecover->Create(playerBody->GetID(), MT_MONO_COLOR_CUBE, 1, 0.2f, 1.0f, false);
        particleRecover->Create(playerBody->GetID(), MT_CUBE_METALGRID, 0.0001f, 0.1f, 1.f, false);
        particleRecover->SetColorBehavior({ 0, 1, 0 }, { 0, 1, 0 }, 1, 0);
        particleRecover->SetTransformBehavior({ 0, 0.1f, 0 }, .5f, { 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1 });
        particleRecover->SetParticleTransform({ 0.5f, 0, 0, 0, 0, .5f, 0, 0, 0, 0, .5f, 0, 0, 0, 0, 1 });

        particleRecoverFuel->Create(playerBody->GetID(), MT_CUBE_METALGRID, 0.0001f, 0.1f, 1.f, false);
        particleRecoverFuel->SetColorBehavior({ 1, 0, 0 }, { 1, 0, 0 }, 1, 0);
        particleRecoverFuel->SetTransformBehavior({ 0, 0.1f, 0 }, .5f, { 0.9f, 0, 0, 0, 0, 0.9f, 0, 0, 0, 0, 0.9f, 0, 0, 0, 0, 1 });
        particleRecoverFuel->SetParticleTransform({ 0.5f, 0, 0, 0, 0, .5f, 0, 0, 0, 0, .5f, 0, 0, 0, 0, 1 });

    }

    if (playerAccentLeft)
    {
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        // Add Components
        playerAccentLeft->AddComponent(trans);
        playerAccentLeft->AddComponent(model);

        // Initialize Components
        trans->Create(playerAccentLeft->GetID(), 1.f, 0.1f, 0.075f, -0.5, 0.70, 0.97);
        model->Create(playerAccentLeft->GetID(), MT_CUBE_DARK);
    }

    if (playerAccentRight)
    {
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        // Add Components
        playerAccentRight->AddComponent(trans);
        playerAccentRight->AddComponent(model);

        // Initialize Components
        trans->Create(playerAccentRight->GetID(), 1.f, 0.1f, 0.075f, -0.5, 0.70, -0.97);
        model->Create(playerAccentRight->GetID(), MT_CUBE_DARK);
    }

    if (playerAccentBack)
    {
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        // Add Components
        playerAccentBack->AddComponent(trans);
        playerAccentBack->AddComponent(model);

        // Initialize Components
        trans->Create(playerAccentBack->GetID(), 0.5f, 0.55f, 0.6f, 1.1, 0.7f, 0);
        model->Create(playerAccentBack->GetID(), MT_CUBE_DARK);
    }

    if (playerIndicator)
    {
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));
        //   Physics* phy = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));

           // Add Components
        playerIndicator->AddComponent(trans);
        playerIndicator->AddComponent(model);
        //   playerIndicator->AddComponent(phy);

           // Initialize Components
        trans->Create(playerIndicator->GetID(), 1, 10, 1, 0, 30, 0);
        model->Create(playerIndicator->GetID(), MT_MONO_COLOR_CUBE);
        //  phy->Create(playerIndicator->GetID(), 1.f, 1.f, 1.f, 0.0f, 0, 6, 0);
    }

    if (playerBodyFront1)
    {
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        // Add Components
        playerBodyFront1->AddComponent(trans);
        playerBodyFront1->AddComponent(model);

        // Initialize Components
        trans->Create(playerBodyFront1->GetID(), 0.5, 0.6f, 0.35, -1.25, 0.1, 0.60);
        model->Create(playerBodyFront1->GetID(), MT_CUBE_METALGRID);
    }

    if (playerBodyFront2)
    {
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        // Add Components
        playerBodyFront2->AddComponent(trans);
        playerBodyFront2->AddComponent(model);

        // Initialize Components
        trans->Create(playerBodyFront2->GetID(), 0.5, 0.6f, 0.35, -1.25, 0.1, -0.60);
        model->Create(playerBodyFront2->GetID(), MT_CUBE_METALGRID);
    }

    if (playerBodyBack && pBodyphys)
    {
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));
        physics = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));

        // Add Components
        playerBodyBack->AddComponent(trans);
        playerBodyBack->AddComponent(model);
        playerBodyBack->AddComponent(physics);

        // Initialize Components
        btVector3 pos(1.05f, 0.1f, 0.f);
        trans->Create(playerBodyBack->GetID(), 1.f, 1.f, 1.f, 0.f, 0.f, 0.f);
        model->Create(playerBodyBack->GetID(), MT_CUBE_METALGRID);
        physics->Create(playerBodyBack->GetID(),
            0.3f, 0.6f, 0.75f,
            0.5f,
            xPos + pos.getX(), yPos + pos.getY(), zPos + pos.getZ()
        );

        // just to fix between the two objects
        pHinge = setHinge(
            *(pBodyphys->box->rigidbody), *(physics->box->rigidbody),
            pivotA, -pos,
            btVector3(1.f, 0.f, 0.f), btVector3(1.f, 0.f, 0.f)
        );

        physics->box->rigidbody->setAngularFactor(btVector3(0.f, 1.f, 0.f)); // it fixes the rigidbod's rotating
    }

    if (playerGunAccentBack)
    {
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        // Add Components
        playerGunAccentBack->AddComponent(trans);
        playerGunAccentBack->AddComponent(model);

        // Initialize Components
        trans->Create(playerGunAccentBack->GetID(), 0.5f, 0.7f, 0.5f, 0.9, 0.7f, 1);
        model->Create(playerGunAccentBack->GetID(), MT_CUBE_DARK);
    }

    if (playerGunAccentBack2)
    {
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        // Add Components
        playerGunAccentBack2->AddComponent(trans);
        playerGunAccentBack2->AddComponent(model);

        // Initialize Components
        trans->Create(playerGunAccentBack2->GetID(), 0.1f, 0.6f, 0.85f, 1.1, 0.7f, -0.2f);
        model->Create(playerGunAccentBack2->GetID(), MT_CUBE_DARK);
    }

    if (playerGunAccentBack3)
    {
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        // Add Components
        playerGunAccentBack3->AddComponent(trans);
        playerGunAccentBack3->AddComponent(model);

        // Initialize Components
        trans->Create(playerGunAccentBack3->GetID(), 0.1f, 0.6f, 0.85f, 0.6, 0.7f, -0.2f);
        model->Create(playerGunAccentBack3->GetID(), MT_CUBE_DARK);
    }

    ////////////////
    // Player's Head
    vec3 headSize(1.5f, 0.3f, 0.3f);
    vec3 headPosOffset(-0.5f, headSize.x, 0.f);
    GameObject* playerHead = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    if (playerHead)
    {
        playerHead->SetName("player_head");
        // Allocate Components
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        pHeadphys = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));
        bulletParticles = dynamic_cast<ParticleEmitter*>(ParticleEmitterManager::GetInstance()->Get(ParticleEmitterManager::GetInstance()->Allocate()));

        // Add Components
        playerHead->AddComponent(trans);
        playerHead->AddComponent(model);
        playerHead->AddComponent(pHeadphys);
        playerHead->AddComponent(bulletParticles);

        bulletParticles->SetColorBehavior({ 1, 1, 1 }, { 1, 1, 1 }, 1, 0);
        bulletParticles->SetTransformBehavior({ 0, 0, 0 }, .1f, { 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1 });
        //bulletParticles->SetParticleTransform({ 0.05f, 0, 0, 0, 0, .05f, 0, 0, 0, 0, .05f, 0, 0, 0, 0, 1 });

        // Initialize Components
        trans->Create(playerHead->GetID(), 1.5f, 1.f, 1.f, 0.f, 0.f, 0.f);
        pHeadphys->Create(playerHead->GetID(), headSize.x, headSize.y, headSize.z,
            1.f,
            xPos + headPosOffset.x, yPos + headPosOffset.y, zPos + headPosOffset.z);
        pHeadphys->box->rigidbody->setGravity(btVector3(0.f, 0.f, 0.f));
        model->Create(playerHead->GetID(), MT_CUBE_METALGRID);
        bulletParticles->Create(playerHead->GetID(), MT_MONO_COLOR_SPHERE, PLAYER_WEAPONE_FIRERATE, 1, 0.2f, false);
        bulletParticles->SetParticleTransform(
            { 0.5f, 0, 0, 0,
              0, 0.5f, 0, 0,
              0, 0, 0.5f, 0,
              1.3f, 0, 1.3f, 1 });
    }

    ////////////
    // Parenting
    if (player && playerBody && playerHead && playerBodyFront1 && playerBodyFront2 && playerBodyBack && playerIndicator
        && playerAccentLeft && playerAccentRight && playerAccentBack && playerGunAccentBack && playerGunAccentBack2 && playerGunAccentBack3)
    {
        PlayerBehavior* pPlayerBehav = dynamic_cast<PlayerBehavior*>(behav);

        // object ID
        ObjectManager::MakeParent(playerHead->GetID(), player->GetID());
        ObjectManager::MakeParent(playerBody->GetID(), player->GetID());

        ObjectManager::MakeParent(playerIndicator->GetID(), player->GetID());
        ObjectManager::MakeParent(playerBodyFront1->GetID(), player->GetID());
        ObjectManager::MakeParent(playerBodyFront2->GetID(), player->GetID());
        ObjectManager::MakeParent(playerAccentLeft->GetID(), player->GetID());
        ObjectManager::MakeParent(playerAccentRight->GetID(), player->GetID());

        // playerBodyBack
        ObjectManager::MakeParent(playerBodyBack->GetID(), player->GetID());
        ObjectManager::MakeParent(playerAccentBack->GetID(), playerBodyBack->GetID());

        ObjectManager::MakeParent(playerGunAccentBack->GetID(), playerHead->GetID());
        ObjectManager::MakeParent(playerGunAccentBack2->GetID(), playerHead->GetID());
        ObjectManager::MakeParent(playerGunAccentBack3->GetID(), playerHead->GetID());
        pPlayerBehav->SetPartIDs(playerHead->GetID(), player->GetID(), playerBody->GetID()); // player = body

        // removes playerHead collision with playerBody
        //pHeadphys->box->rigidbody->setCollisionFlags(pBodyphys->box->rigidbody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

        pivotA = btVector3(0.f, 0.f, 0.f);
        pivotB = btVector3(-headPosOffset.x, -headPosOffset.y, -headPosOffset.z);

        // body (rigid) - horizontal hinge - link (rigid) - vertical hinge  - head (rigid)
        {
            GameObject* linkObj = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
            if (linkObj)
            {
                linkObj->SetName("player_link_body_head");

                // Physics
                Physics* linkObjPhys = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));
                linkObj->AddComponent(linkObjPhys);

                linkObjPhys->Create(linkObj->GetID(), 1.f, 1.f, 1.f,
                    1.f,
                    0.f, 0.f, 0.f);
                linkObjPhys->box->rigidbody->setGravity(btVector3(0.f, 0.f, 0.f));

                // removes linkObjPhys collision with body and head
                linkObjPhys->box->ignoreCollisionWithObject();


                // Horizontal hinge
                // this MUST be added last rather than other hinges which connected to the body
                pHinge = setHinge(
                    *(pBodyphys->box->rigidbody), *(linkObjPhys->box->rigidbody),
                    pivotA, pivotA,
                    btVector3(0.f, 1.f, 0.f), btVector3(0.f, 1.f, 0.f)
                );

                pHinge->enableAngularMotor(true, 0, 100000);

                // it makes the head is not bouncing when the tank starts to move
                pHinge->setParam(BT_CONSTRAINT_CFM, 0.00001f);
                pHinge->setParam(BT_CONSTRAINT_STOP_CFM, 0.00001f);
                pHinge->setParam(BT_CONSTRAINT_ERP, 0.8f);
                pHinge->setParam(BT_CONSTRAINT_STOP_ERP, 0.8f);

                // Vertical hinge
                pHinge = setHinge(
                    *(linkObjPhys->box->rigidbody), *(pHeadphys->box->rigidbody),
                    pivotA, pivotB,
                    btVector3(0.f, 0.f, 1.f), btVector3(0.f, 0.f, 1.f)
                );

                pHinge->enableAngularMotor(true, 0, 100000);

                // it makes the head is not bouncing when the tank starts to move
                pHinge->setParam(BT_CONSTRAINT_CFM, 0.00001f);
                pHinge->setParam(BT_CONSTRAINT_STOP_CFM, 0.00001f);
                pHinge->setParam(BT_CONSTRAINT_ERP, 0.8f);
                pHinge->setParam(BT_CONSTRAINT_STOP_ERP, 0.8f);
            }
        }


    }
    else {
        printf("Can't make a player compound.");
        //ObjectManager::DestroyObject(playerHead->GetID());
        //ObjectManager::DestroyObject(playerBody->GetID());
        //ObjectManager::DestroyObject(player->GetID());
        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    return player->GetID();
}

int ObjectFactory::CreateFlag() {
    GameObject* flag = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    if (flag) {
        flag->SetName("flag");

        // the flag
        //Transform* flagTransform = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        //Physics* flagPhysics = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));;
        //ModelComponent* flagModel = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));
        Behavior* behav = AllocateBehavior<FlagBehavior>(BT_FLAG, flag->GetID());

        //flag->AddComponent(flagTransform);
        //flag->AddComponent(flagPhysics);
        //flag->AddComponent(flagModel);
        flag->AddComponent(behav);

        //flagTransform->Create(flag->GetID(), 1, 1, 1, 0, 0, 0);
        //flagPhysics->Create(flag->GetID(), 1.0f, 1.0f, 1.0f, 0.f, 0.f, -0.5f, 0.f);
        //flagModel->Create(flag->GetID(), MT_FLAG);
        behav = dynamic_cast<FlagBehavior*>(behav->Init(flag->GetID(), BT_FLAG)); //  initializes the base class, 
        dynamic_cast<FlagBehavior*>(behav)->Create(flag->GetID());
        /*
        // flag's indicator
        GameObject* pIndFlag = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
        if (pIndFlag)
        {
          pIndFlag->SetName("flag_indicator");
          Transform* pIndTrans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
          Physics* pIndPhys = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));;
          ModelComponent* pIndModel = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

          pIndFlag->AddComponent(pIndTrans);
          pIndFlag->AddComponent(pIndPhys);
          pIndFlag->AddComponent(pIndModel);

          pIndTrans->Create(pIndFlag->GetID(), 1, 1, 1, 0, 0, 0);
          pIndPhys->Create(pIndFlag->GetID(),
            1.0f, 10.0f, 1.0f,
            0.f,
            0.f, 20.f, 0.f);
          pIndPhys->box->rigidbody->setCollisionFlags(btCollisionObject::CO_GHOST_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
          pIndModel->Create(pIndFlag->GetID(), MT_FLAG);

          ObjectManager::MakeParent(pIndFlag->GetID(), flag->GetID());
        }
        */
        return flag->GetID();
    }
    else {
        return -1;
    }

}

int ObjectFactory::CreatePowerUp() {
    int n = ObjectManager::GetInstance()->Allocate();
    GameObject* PU = ObjectManager::GetObjectByID(n);
    PU->SetName("powerUp");
    if (PU) {
        char temp[10];
        itoa(n, temp, 10);
        tn_object->AddWidget(new PAGuiTransform(temp, n));
        Transform* powerUpTransform = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        Physics* powerUpPhysics = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));
        ModelComponent* powerUpModel = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));
        Behavior* behav = AllocateBehavior<PowerUpsBehavior>(BT_POWERUP, PU->GetID());

        PU->AddComponent(powerUpTransform);
        PU->AddComponent(powerUpPhysics);
        PU->AddComponent(powerUpModel);
        PU->AddComponent(behav);

        powerUpTransform->Create(PU->GetID(), 1, 1, 1, 0, 0, 0);
        powerUpPhysics->Create(PU->GetID(), 1.0f, 1.0f, 1.0f, 1.f, 0, 0, 0);
        powerUpPhysics->box->rigidbody->setAngularFactor(btVector3(0.f, 0.f, 0.f));
        powerUpPhysics->box->rigidbody->setGravity(btVector3(0.f, 0.f, 0.f));
        powerUpModel->Create(PU->GetID(), MT_MONO_COLOR_SPHERE);
        size_t id = powerUpModel->GetInstance();
        ModelManager::GetModel(MT_MONO_COLOR_SPHERE)->SetColor(id, 0.f, 0.f, 1.f);
        ModelManager::GetModel(MT_MONO_COLOR_SPHERE)->SetAlpha(id, 0.5f);

        ParticleEmitter* pe = dynamic_cast<ParticleEmitter*>(ParticleEmitterManager::GetInstance()->Get(ParticleEmitterManager::GetInstance()->Allocate()));
        PU->AddComponent(pe);
        pe->Create(PU->GetID(), MT_CUBE_METALGRID, 0.0001f, 0.1f, 1.f, true);
        pe->SetColorBehavior({ 0, 0, 1 }, { 0, 1, 1 }, 1, 0);
        pe->SetTransformBehavior({ 0, 0, 0 }, 1.f, { 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1 });
        pe->SetParticleTransform({ 0.05f, 0, 0, 0, 0, .05f, 0, 0, 0, 0, .05f, 0, 0, 0, 0, 1 });


        behav = dynamic_cast<PowerUpsBehavior*>(behav->Init(PU->GetID(), BT_POWERUP)); //  initializes the base class, 
        dynamic_cast<PowerUpsBehavior*>(behav)->Create(PU->GetID());

        return PU->GetID();
    }
    else {
        return -1;
    }

}

int ObjectFactory::CreateAstar() {
    GameObject* astar = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    if (astar) {
        astar->SetName("astar");
        //Transform* astarTransform = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        //Physics* astarPhysics = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));;
        //ModelComponent* astarModel = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));
        AstarBehavior* behav = AllocateBehavior<AstarBehavior>(BT_ASTAR, astar->GetID()); //  initializes the base class, 

        //astar->AddComponent(astarTransform);
        //astar->AddComponent(astarPhysics);
        //astar->AddComponent(astarModel);
        astar->AddComponent(behav);

        //astarTransform->Create(astar->GetID(), 1, 1, 1, -50, 4, -50);
        //astarPhysics->Create(astar->GetID(), 1.0f, 1.0f, 1.0f, 0.f, 0.f, -0.5f, 0.f);
        //astarModel->Create(astar->GetID(), MT_FLAG);
        behav->Create(astar->GetID());
        return astar->GetID();
    }
    else {
        return -1;
    }

}

int ObjectFactory::CreateGauge(const int ID, const int targetObjectID) {
    int n = ObjectManager::GetInstance()->Allocate();
    GameObject* gauge = ObjectManager::GetObjectByID(n);
    if (gauge) {
        char temp[10];
        itoa(n, temp, 10);
        tn_object->AddWidget(new PAGuiTransform(temp, n));
        gauge->SetName("gauge");
        Transform* gaugeTransform = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        Physics* gaugePhysics = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));;
        ModelComponent* gaugeModel = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));
        GaugeBehavior* behav = dynamic_cast<GaugeBehavior*>(BehaviorManager::GetInstance()->Get(BehaviorManager::GetInstance()->Allocate()));
        behav = dynamic_cast<GaugeBehavior*>(behav->Init(gauge->GetID(), BT_GAUGE)); //  initializes the base class, 

        gauge->AddComponent(gaugeTransform);
        gauge->AddComponent(gaugePhysics);
        gauge->AddComponent(gaugeModel);
        gauge->AddComponent(behav);

        gaugeTransform->Create(gauge->GetID(), 1, 1, 1, 0, 0, 0);
        gaugePhysics->Create(gauge->GetID(), 5.0f, 0.2f, 1.0f, 0.f, 0.f, 4.5f, 0.f);
        gaugeModel->Create(gauge->GetID(), MT_GAUGE);
        behav->Create(gauge->GetID());
        behav->ID = ID;
        behav->targetObjectID = targetObjectID;
        return gauge->GetID();
    }
    else {
        return -1;
    }

}

int ObjectFactory::CreateAI(float xPos, float yPos, float zPos, bool moveX) {

    Transform* trans = nullptr;
    AIBehavior* behav = nullptr;
    ModelComponent* model = nullptr;

    Physics* pHeadphys = nullptr;
    Physics* pBodyphys = nullptr;

    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

  ////////////////
  // AI's Body
    int a = ObjectManager::GetInstance()->Allocate();
    GameObject* body = ObjectManager::GetObjectByID(a);
    if (body)
    {
        char temp[10];
        itoa(a, temp, 10);
        tn_object->AddWidget(new PAGuiTransform(temp, a));
        body->SetName("player_AI_body");
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        body->AddComponent(trans);
        body->AddComponent(model);

        trans->Create(body->GetID(), 1, 0.75f, 1, 0, 0, 0);
        model->Create(body->GetID(), MT_CUBE_METALGRID);
    }

    ////////////////
    // AI's Head
    vec3 headSize(1.5f, 0.3f, 0.3f);
    vec3 headPosOffset(-0.5f, headSize.x, 0.f);
    int b = ObjectManager::GetInstance()->Allocate();
    GameObject* head = ObjectManager::GetObjectByID(b);
    if (head)
    {
        char temp[10];
        itoa(b, temp, 10);
        tn_object->AddWidget(new PAGuiTransform(temp, b));
        head->SetName("player_AI_head");
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        pHeadphys = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));
        model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));


        head->AddComponent(trans);
        head->AddComponent(model);
        head->AddComponent(pHeadphys);

        trans->Create(head->GetID(), 1.5f, 1.f, 1.f, 0.f, 0.f, 0.f);
        pHeadphys->Create(head->GetID(), headSize.x, headSize.y, headSize.z,
            1.f,
            xPos + headPosOffset.x, yPos + headPosOffset.y, zPos + headPosOffset.z);
        pHeadphys->box->rigidbody->setGravity(btVector3(0.f, 0.f, 0.f));
        model->Create(head->GetID(), MT_CUBE_METALGRID);
    }


    /////////////////////////
    // Player Object + Camera
    // GameObject* AIPlayer = ObjectManager::CreateObject("player_AI", CT_TRANSFORM | CT_PHYSICS | CT_BEHAVIOR);
    int c = ObjectManager::GetInstance()->Allocate();
    GameObject* AIPlayer = ObjectManager::GetObjectByID(c);
    if (AIPlayer)
    {
        char temp[10];
        itoa(c, temp, 10);
        tn_object->AddWidget(new PAGuiTransform(temp, c));
        AIPlayer->SetName("player_AI");
        trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        pBodyphys = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));
        behav = AllocateBehavior<AIBehavior>(BT_AI, AIPlayer->GetID());

        AIPlayer->AddComponent(trans);
        AIPlayer->AddComponent(pBodyphys);
        AIPlayer->AddComponent(behav);

        trans->Create(AIPlayer->GetID(), 1, 1, 1, 0, 0, 0);
        pBodyphys->Create(AIPlayer->GetID(),
            1.f, 1.f, 1.f,
            0.f, // this is a towar which don't move.
            xPos, yPos, zPos);
        pBodyphys->box->rigidbody->setAngularFactor(btVector3(0.f, 0.f, 0.f)); // it fixes the body's rotating
        behav = dynamic_cast<AIBehavior*>(behav->Init(AIPlayer->GetID(), BT_AI)); //  initializes the base class, 
        dynamic_cast<AIBehavior*>(behav)->Create(AIPlayer->GetID());
        dynamic_cast<AIBehavior*>(behav)->moveXaxis = moveX;
    }

    ////////////
    // Parenting
    if (AIPlayer && head && body)
    {
        ObjectManager::MakeParent(head->GetID(), AIPlayer->GetID());
        ObjectManager::MakeParent(body->GetID(), AIPlayer->GetID());
        behav->SetPartIDs(head->GetID(), AIPlayer->GetID()); // AIPlayer = body

        btVector3 pivotA = btVector3(0.f, 0.f, 0.f);
        btVector3 pivotB = btVector3(-headPosOffset.x, -headPosOffset.y, -headPosOffset.z);
        btHingeAccumulatedAngleConstraint* pHinge = nullptr;

        // body (rigid) - horizontal hinge - link (rigid) - vertical hinge  - head (rigid)
        {
            //GameObject* linkObj = ObjectManager::CreateObject("player_AI_link_body_head", CT_PHYSICS);
            GameObject* linkObj = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
            if (linkObj)
            {
                // Physics
                Physics* linkObjPhys = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));
                linkObj->AddComponent(linkObjPhys);
                linkObjPhys->Create(linkObj->GetID(), 1.f, 1.f, 1.f,
                    1.f,
                    0.f, 0.f, 0.f);
                linkObjPhys->box->rigidbody->setGravity(btVector3(0.f, 0.f, 0.f));

                // removes linkObjPhys collision with body and head
                linkObjPhys->box->ignoreCollisionWithObject();


                // Horizontal hinge
                pHinge = setHinge(
                    *(pBodyphys->box->rigidbody), *(linkObjPhys->box->rigidbody),
                    pivotA, pivotA,
                    btVector3(0.f, 1.f, 0.f), btVector3(0.f, 1.f, 0.f)
                );

                pHinge->enableAngularMotor(true, 0, 100000);

                // it makes the head is not bouncing when the tank starts to move
                pHinge->setParam(BT_CONSTRAINT_CFM, 0.00001f);
                pHinge->setParam(BT_CONSTRAINT_STOP_CFM, 0.00001f);
                pHinge->setParam(BT_CONSTRAINT_ERP, 0.8f);
                pHinge->setParam(BT_CONSTRAINT_STOP_ERP, 0.8f);

                // Vertical hinge
                pHinge = setHinge(
                    *(linkObjPhys->box->rigidbody), *(pHeadphys->box->rigidbody),
                    pivotA, pivotB,
                    btVector3(0.f, 0.f, 1.f), btVector3(0.f, 0.f, 1.f)
                );

                pHinge->enableAngularMotor(true, 0, 100000);

                // it makes the head is not bouncing when the tank starts to move
                pHinge->setParam(BT_CONSTRAINT_CFM, 0.00001f);
                pHinge->setParam(BT_CONSTRAINT_STOP_CFM, 0.00001f);
                pHinge->setParam(BT_CONSTRAINT_ERP, 0.8f);
                pHinge->setParam(BT_CONSTRAINT_STOP_ERP, 0.8f);
            }
        }
    }
    else {
        printf("Can't make a player compound.");
        return -1;
    }

    return AIPlayer->GetID();
}

int ObjectFactory::CreateBox(float scaleX, float scaleY, float scaleZ, float mass, float offsetX, float offsetY, float offsetZ, ModelType type) {
    int n = ObjectManager::GetInstance()->Allocate();
    GameObject* block = ObjectManager::GetObjectByID(n);
    if (block) {
        char temp[10];
        itoa(n, temp, 10);
        tn_object->AddWidget(new PAGuiTransform(temp, n));
        block->SetName("box");
        Transform* trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        Physics* phys = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));;
        ModelComponent* blockModel = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        block->AddComponent(trans);
        block->AddComponent(phys);
        block->AddComponent(blockModel);

        trans->Create(block->GetID(), 1, 1, 1, 0, 0, 0);
        phys->Create(block->GetID(), scaleX, scaleY, scaleZ, mass, offsetX, offsetY, offsetZ);
        blockModel->Create(block->GetID(), type);

        return block->GetID();
    }
    else {
        return -1;
    }
}

int ObjectFactory::CreateChildBox(float scaleX, float scaleY, float scaleZ, float offsetX, float offsetY, float offsetZ) {
    int n = ObjectManager::GetInstance()->Allocate();
    GameObject* block = ObjectManager::GetObjectByID(n);
    if (block) {
        char temp[10];
        itoa(n, temp, 10);
        tn_object->AddWidget(new PAGuiTransform(temp, n));
        block->SetName("box");
        Transform* blockTransform = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        ModelComponent* blockModel = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        block->AddComponent(blockTransform);
        block->AddComponent(blockModel);

        blockTransform->Create(block->GetID(), scaleX, scaleY, scaleZ, offsetX, offsetY, offsetZ);
        blockModel->Create(block->GetID(), MT_CUBE_METALGRID);
        return block->GetID();
    }
    else {
        return -1;
    }
}

int ObjectFactory::CreateBullet(int shooter,
    float posX, float posY, float posZ,
    float dirX, float dirY, float dirZ,
    bool explode,
    bool damagePlayer,
    float bulletSize,
    float bulletSpeed)
{
    GameObject* bullet = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    if (bullet) {
        bullet->SetName("bullet");
        Transform* trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        Physics* phys = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));;
        ModelComponent* blockModel = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));
        Behavior* behav = AllocateBehavior<BulletBehavior>(BT_BULLET, bullet->GetID());

        ParticleEmitter* pe = dynamic_cast<ParticleEmitter*>(ParticleEmitterManager::GetInstance()->Get(ParticleEmitterManager::GetInstance()->Allocate()));
        bullet->AddComponent(pe);
        pe->Create(bullet->GetID(), MT_MONO_COLOR_SPHERE, -1, -1, 0.4f, true);
        pe->SetColorBehavior(vec3(0.1, 0.1, 0.1), vec3(0.0, 0.0, 0.0), 0.5f, 0);
        pe->SetParticleTransform(
            { 0.1f, 0, 0, 0,
              0, 0.1f, 0, 0,
              0, 0, 0.1f, 0,
              0, 0, 0, 1 });

        bullet->AddComponent(trans);
        bullet->AddComponent(phys);
        bullet->AddComponent(blockModel);
        bullet->AddComponent(behav);

        trans->Create(bullet->GetID(), 0.5f, 0.5f, 0.5f, 0, 0, 0);
        phys->Create(bullet->GetID(), bulletSize, bulletSize, bulletSize, bulletSize * 2.0f, posX, posY, posZ);
        blockModel->Create(bullet->GetID(), MT_SPHERE);
        behav = dynamic_cast<BulletBehavior*>(behav->Init(bullet->GetID(), BT_BULLET)); //  initializes the base class, 
        dynamic_cast<BulletBehavior*>(behav)->Create(shooter, explode, damagePlayer);

        phys->SetImpulse(dirX * bulletSpeed, dirY * bulletSpeed, dirZ * bulletSpeed);

        // callback for when bullet hits something
        phys->box->addCollisionCallback((OnCollision*)&BulletCollidesWithAnything_CB);

        return bullet->GetID();
    }
    else {
        return -1;
    }
}
int ObjectFactory::CreateFuelDepot(float offsetX, float offsetY, float offsetZ) {
    GameObject* fuelDepot = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    float depotLenght = 9.f;
    float depotWidth = 9.f;
    float depotHieght = 0.01f;


    if (fuelDepot) {
        fuelDepot->SetName("fuelDepot");
        Transform* trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        Physics* phys = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));
        ModelComponent* model = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));
        ParticleEmitter* pe = dynamic_cast<ParticleEmitter*>(ParticleEmitterManager::GetInstance()->Get(ParticleEmitterManager::GetInstance()->Allocate()));
        FuelDepotBehavior* behav = dynamic_cast<FuelDepotBehavior*>(AllocateBehavior<FuelDepotBehavior>(BT_FUELDEPOT, fuelDepot->GetID()));

        fuelDepot->AddComponent(trans);
        fuelDepot->AddComponent(phys);
        fuelDepot->AddComponent(model);
        fuelDepot->AddComponent(pe);
        fuelDepot->AddComponent(behav);

        phys->Create(fuelDepot->GetID(), depotWidth, depotHieght, depotLenght, 0.f, offsetX, offsetY, offsetZ);
        trans->Create(fuelDepot->GetID(), 1, 1, 1, offsetX, offsetY, offsetZ);
        phys->box->rigidbody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
        phys->box->rigidbody->setActivationState(DISABLE_SIMULATION);
        model->Create(fuelDepot->GetID(), MT_FUEL_DEPOT);
        pe->Create(fuelDepot->GetID(), MT_MONO_COLOR_SPHERE, 0.1f, 0.1f, 0.5f, true);
        behav = dynamic_cast<FuelDepotBehavior*>(behav->Init(fuelDepot->GetID(), BT_FUELDEPOT));
        behav->Create(fuelDepot->GetID(), 200, 0.14f, 1.f);
        // set the model color
      //  size_t id = model->GetInstance();
        //ModelManager::GetModel(MT_FUEL_DEPOT)->SetColor(id, 0.f, 1.f, 0.f);

        //setup particle behaviors
        pe->SetTransformBehavior({ 0,.3,0 }, 0.05, { .98,0,0,0,0,.98,0,0,0,0,.98,0,0,0,0,1 });
        pe->SetColorBehavior({ .2,.5,.2 }, { 0,1,0 }, 1, 0);

        return fuelDepot->GetID();
    }
    return -1;
}


int ObjectFactory::CreateTutorial() {
    GameObject* tutorial = ObjectManager::GetObjectByID(ObjectManager::GetInstance()->Allocate());
    if (tutorial) {
        Transform* trans = dynamic_cast<Transform*>(TransformManager::GetInstance()->Get(TransformManager::GetInstance()->Allocate()));
        Physics* phys = dynamic_cast<Physics*>(PhysicsManager::GetInstance()->Get(PhysicsManager::GetInstance()->Allocate()));
        ModelComponent* blockModel = dynamic_cast<ModelComponent*>(ModelManager::GetInstance()->Get(ModelManager::GetInstance()->Allocate()));

        tutorial->AddComponent(trans);
        tutorial->AddComponent(phys);
        tutorial->AddComponent(blockModel);

        trans->Create(tutorial->GetID(), 1, 1, 1, 0, 0, 0);
        phys->Create(tutorial->GetID(), .2f, 4.f, 8.f, 0.f, 60, 4, 75);
        blockModel->Create(tutorial->GetID(), MT_TUTORIAL);
    }

    return tutorial->GetID();
}
