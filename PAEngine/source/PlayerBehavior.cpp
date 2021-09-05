/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/
#include "pch.h"
#include "PlayerBehavior.h"
#include "PlayerConfig.h"
#include "Transform.h" 
#include "ChildTransform.h"
#include "ModelManager.h"
#include "ModelComponent.h"
#include "ObjectManager.h"
#include "ObjectFactory.h"
#include "CameraManager.h"
#include "ServiceLocator.h"
#include "EventSystem.h"
#include "Physics.h"
#include "EnvironmentManager.h"
#include "PlayerController.h"
#include "ParticleEmitter.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Physics.h"
#include "BulletBehavior.h"
#include "FlagBehavior.h"
#include "AIBehavior.h"
#include "PlayerAttributesController.h"
#include "EnvironmentManager.h"
#include "AstarBehavior.h"

// player behavior states
#include "PlayerBehavStateDefault.h"
#include "PlayerBehavStateRapidFire.h"
#include "PlayerBehavStateDead.h"

void GroundCollidesWithPlayer_CB(unsigned myID, unsigned otherID, const std::string& bullet)
{
    EnvironmentManager* enviro = EnvironmentManager::getEnvironment();
    float row = enviro->gridAmount;
    float worldWidth = enviro->getWorldSize();
    float width = worldWidth / row;
    width /= 2;

    GameObject* player = ObjectManager::GetObjectByID(myID);
    PlayerBehavior* behav = player->GetComponent<PlayerBehavior>();
    GameObject* groundObj = ObjectManager::GetObjectByID(otherID);
    AstarBehavior* groundBehav = groundObj->GetComponent<AstarBehavior>();

    /*Transform* playerTransform = player->GetComponent<Transform>();
    float playerX = playerTransform->GetPositionX();
    float playerZ = playerTransform->GetPositionZ();


    groundBehav->hit = myID;

    Transform* groundTransform = groundObj->GetComponent<Transform>();
    float groundX = groundTransform->GetPositionX();
    float groundZ = groundTransform->GetPositionZ();

    float disX = groundX - playerX;
    float disZ = groundZ - playerZ;
    //printf("%f, %f\n", width, disX * disX + disZ * disZ);
    if (disX * disX + disZ * disZ > width * width)
    {
      //return;
    }*/

    GameObject* flag = ObjectManager::GetObjectsByName("flag")[0];
    FlagBehavior* flagbehav = flag->GetComponent<FlagBehavior>();
    if (flagbehav->GetWinTime() < 0)
    {
        return;
    }

    if (behav->fuel < 1)
    {
        return;
    }

    int id = behav->GetPlayerID();
    int oid = groundBehav->owner;

    if (oid == id)
    {
        return;
    }

    switch (id)
    {
    case 0:
        ++flagbehav->red;
        break;
    case 1:
        ++flagbehav->blue;
        break;
    case 2:
        ++flagbehav->yellow;
        break;
    case 3:
        ++flagbehav->green;
        break;
    default:
        return;
    }

    switch (oid)
    {
    case 0:
        --flagbehav->red;
        break;
    case 1:
        --flagbehav->blue;
        break;
    case 2:
        --flagbehav->yellow;
        break;
    case 3:
        --flagbehav->green;
        break;
    default:
        break;
    }

    ParticleEmitter* pe = groundObj->GetComponent<ParticleEmitter>(0);
    pe->SetColorBehavior(behav->colorOrigin, behav->colorOrigin, 1, 0);
    pe->SetActive(true);
    pe = groundObj->GetComponent<ParticleEmitter>(1);
    pe->SetColorBehavior(behav->colorOrigin, behav->colorOrigin, 1, 0);
    pe->SetActive(true);

    groundBehav->ParticleTimer = 0.5f;

    behav->fuel -= behav->fuelTileCost;


    groundBehav->owner = id;

    //printf("%i, %i, %i, %i\n", flagbehav->red, flagbehav->blue, flagbehav->yellow, flagbehav->green);

    vec3 color = behav->colorOrigin;
    groundBehav->oldcolor = groundBehav->targetcolor;
    groundBehav->targetcolor = color;
    groundBehav->changingColor = true;
    //groundBehav->ChangeColor();
    //ModelComponent* pModel = groundObj->GetComponent<ModelComponent>();
    //if (pModel != nullptr)
    //{
        //ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
            //color.x, color.y, color.z);
    //}

    EventSystem::SendEvent(ET_SOUND_OS_PAINT, nullptr, 0);
}

void BulletsCollidesWithPlayer_CB(unsigned myID, unsigned otherID, const std::string& bullet)
{
    PlayerBehavior* behav = ObjectManager::GetObjectByID(myID)->GetComponent<PlayerBehavior>();
    BulletBehavior* bulletbehav = ObjectManager::GetObjectByID(otherID)->GetComponent<BulletBehavior>();
    if (bulletbehav->GetShooterID() != myID && !bulletbehav->hit)
    {
        if (behav)
        {
            if (behav->isInvincible())
            {
                return;
            }
            if (bulletbehav->WillDamagePlayer())
            {
                --behav->HP;
                bulletbehav->hit = true;
                PlayerBehavior* behavior = ObjectManager::GetObjectByID(bulletbehav->GetShooterID())->GetComponent<PlayerBehavior>();
                if (behavior)
                {
                    behav->shotBy = behavior->GetPlayerID();
                }
                else
                {
                    behav->shotBy = ObjectManager::GetObjectByID(bulletbehav->GetShooterID())->GetComponent<AIBehavior>()->GetOwner();
                }
            }

            CameraComponent* cam = ObjectManager::GetObjectByID(myID)->GetComponent<CameraComponent>();
            cam->StartShake(30, 30, 0.15f);
            behav->StartGamepadVibration(0.1f, 075.f, 0.75f);

            // Minsu moved thoese codes which will be excuted when the player dead.
            // please implement that kind of code in PlayerBehavStateDead::Init function.
            // if (behav->HP < 1)
        }
    }
}

PlayerBehavior::PlayerBehavior()
    : gamepad_(nullptr), m_playerID(-1), forces_(nullptr) {
    AddBehaviorState(new PlayerBehavStateDefault(this));
    AddBehaviorState(new PlayerBehavStateRapidFire(this));
    AddBehaviorState(new PlayerBehavStateDead(this));
    AddBehaviorState(new PlayerBigShot(this));
}

PlayerBehavior::~PlayerBehavior() {
    gamepad_ = nullptr;
    forces_ = nullptr;
}

void PlayerBehavior::Create(PAEngine::Gamepad* gamepad, int parentID) {
    gamepad_ = gamepad;
    m_playerID = gamepad->GetPlayerID();
    HP = 5;
    respawnTimer = 0;

    // X: 90 degree, Y: -5 degree (it's for the physics world basis)
    currentAimRadianX = M_PI_2;
    currentAimRadianY = convert2PositiveRadian(-5 * M_PI / 180.f);

    moveInputDirectionZ = 0.f;
    moveInputDirectionX = 0.f;

    vibrateDuration = 0.f;
    currentVibrateDuration = 0.f;

    moveVelocity = PLAYER_MOVEMENT_SPEED;
    weaponeFirerate = PLAYER_WEAPONE_FIRERATE;
    bigShotFireRate = PLAYER_BIG_SHOT_FIRERATE;
    isTwinkling = false;
    colorOrigin = vec3(1.f, 0.f, 0.f); // default: white
    GameObject* thisObject = ObjectManager::GetObjectByID(partBodyID);


    ChangeState(PBS_PLAYING);

    GameObject* player = ObjectManager::GetObjectByID(parentID);
    if (player) {
        Physics* phys = player->GetComponent<Physics>();
        EventSystem::SendEvent(ET_SOUND_L_START_PLAYERMOVE, phys->getVelocity(), sizeof(float*));
    }

    player->GetComponent<Physics>()->box->addCollisionCallback((OnCollision*)&GroundCollidesWithPlayer_CB, "ground");
    player->GetComponent<Physics>()->box->addCollisionCallback((OnCollision*)&BulletsCollidesWithPlayer_CB, "bullet");
}

void PlayerBehavior::playerUpdate(const float dt)
{
    GameObject* thisObject = ObjectManager::GetObjectByID(GetParentID());
    Transform* transform = thisObject->GetComponent<Transform>();
    Physics* physics = thisObject->GetComponent<Physics>();
    GameObject* indicator = nullptr;

    // indecate its HP by chainging parts' color
    for (auto childObj : thisObject->GetChildren())
    {
        GameObject* partObj = ObjectManager::GetObjectByID(childObj);
        if (partObj->GetName() == "player_indicator")
        {
            indicator = partObj;
        }

        if (!(partObj->GetName() == "player_body_Front_1" || partObj->GetName() == "player_body_Front_2" || partObj->GetName() == "player_body_Back" || partObj->GetName() == "player_body" || partObj->GetName() == "player_head"))
        {
            continue;
        }

        ModelComponent* pModel = ObjectManager::GetObjectByID(childObj)->GetComponent<ModelComponent>();
        if (pModel != nullptr)
        {
            //ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
              //colorOrigin.x - (1.0f - (float) HP / 5.0f), colorOrigin.y - (1.0f - (float) HP / 5.0f), colorOrigin.z - (1.0f - (float) HP / 5.0f));
            ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
                ((float)HP / 5.0f), ((float)HP / 5.0f), ((float)HP / 5.0f));
        }

        //if (indicator != nullptr)
        //{
        //    Transform* trans = indicator->GetComponent<Transform>();
        //    Physics* phy = indicator->GetComponent<Physics>();

        //    if (trans->GetPositionY() != 2)
        //        phy->SetPositionY(2);
        //}
    }


    PowerUp();
    if (recover)
    {
        ++recoverTimer;
        if (recoverTimer > 60)
        {
            ParticleEmitter* pe = ObjectManager::GetObjectByID(partBodyIDParticle)->GetComponent<ParticleEmitter>(1);
            pe->SetActive(false);
            recover = false;
            recoverTimer = 0;
        }
    }

    // DEAD
    if (HP <= 0)
    {
        thisObject->GetComponent<Behavior>()->ChangeState(PBS_DEAD);

        // After this dead! There must be no additional action on this object!!
        // If you need to add other actions on its dead, implement in its dead state, PlayerBehavStateDead.cpp.
        return;
    }

    // other elemental systems for the tank when ALIVE!
    int playerID = -1;

    UpdateInput(playerID, thumbLeftX, thumbLeftY, thumbRightX, thumbRightY);

    CameraMove(playerID, thumbRightX, thumbRightY);
    TankMove(playerID, thumbLeftX, thumbLeftY, thumbRightX, thumbRightY);
    Twinkling(dt);
}

void PlayerBehavior::Destroy() {
    gamepad_ = nullptr;
    EventSystem::SendEvent(ET_SOUND_L_END_PLAYERMOVE, nullptr, 0);
    Behavior::Shutdown();
}

void PlayerBehavior::SetMaxVelocity(const float maxVel)
{
    ObjectManager::GetObjectByID(partBodyID)->GetComponent<Physics>()->SetMaxVelocity(maxVel);
}

const float PlayerBehavior::GetMaxVelocity()
{
    return ObjectManager::GetObjectByID(partBodyID)->GetComponent<Physics>()->GetMaxVelocity();
}

void PlayerBehavior::Shoot(const float bulletSize, const float bulletVelcocity, bool explode, bool damagePlayer, const float recoil)
{

    // player
    Transform* trans = ObjectManager::GetObjectByID(GetParentID())->GetComponent<Transform>();

    // create a bullet
    const int bulletID = ObjectFactory::CreateBullet(GetParentID(),
        trans->GetPositionX() - (currentAimUnitVecX * 2), trans->GetPositionY() + 1.5, trans->GetPositionZ() - (currentAimUnitVecY * 2), // position
        -currentAimUnitVecX, currentAimUnitVecXVertical * 1.5f, -currentAimUnitVecY, // direction
        explode, damagePlayer,
        bulletSize, bulletVelcocity);

    // chainging player's color
    ModelComponent* pBulletModel = ObjectManager::GetObjectByID(bulletID)->GetComponent<ModelComponent>();
    if (pBulletModel != nullptr)
    {
        vec3 color;

        if (explode)
            color = vec3(0.f); // black
        else
            color = colorOrigin; // player color

        ModelManager::GetModel(pBulletModel->GetModeltype())->SetColor(pBulletModel->GetInstance(),
            color.x,
            color.y,
            color.z);
    }

    // player's head
    Physics* headPhys = ObjectManager::GetObjectByID(partHeadID)->GetComponent<Physics>();
    headPhys->SetImpulse(currentAimUnitVecX * recoil, 0.f, currentAimUnitVecY * recoil);

    const float revRecoilPower = recoil * -0.5f;

    // player's body
    Physics* bodyPhys = ObjectManager::GetObjectByID(partBodyID)->GetComponent<Physics>();
    bodyPhys->SetImpulse(currentAimUnitVecX * revRecoilPower, 0.f, currentAimUnitVecY * revRecoilPower);

    if (explode) {
        StartGamepadVibration(0.1f, 0.5f, 0.5f);
        EventSystem::SendEvent(ET_SOUND_OS_PLAYERSHOOT, nullptr, 0);
    }
    else {
        StartGamepadVibration(0.1f, 0.3f, 0.3f);
        EventSystem::SendEvent(ET_SOUND_OS_RAPIDFIRE, nullptr, 0);
    }
}

void PlayerBehavior::StartGamepadVibration(const float duration, const float normalizedLeftSpeed, const float normalizedRightSpeed)
{
    vibrateDuration = duration;
    currentVibrateDuration = 0.f;

    // immediately excute the vibrating function
    gamepad_->Vibrate(normalizedLeftSpeed, normalizedRightSpeed);
}

void PlayerBehavior::UpdateInput(int& playerID, float& thumbLeftX, float& thumbLeftY, float& thumbRightX, float& thumbRightY) {

    // for the keyboard-input compatibility (to address a view-direction moving issue)
    moveInputDirectionX_keyboard = moveInputDirectionX;
    moveInputDirectionZ_keyboard = moveInputDirectionZ;
    moveInputDirectionX = ((moveInputDirectionZ_keyboard * -currentAimUnitVecY) + (moveInputDirectionX_keyboard * currentAimUnitVecX));
    moveInputDirectionZ = ((moveInputDirectionZ_keyboard * currentAimUnitVecX) + (moveInputDirectionX_keyboard * currentAimUnitVecY));

    // Null Gamepad, then use Keyboard and Mouse
    if (!gamepad_)
    {
        playerID = 0;

        //if (thumbRightX > 2.f)
        //  thumbRightX = 2.f;
        //else if (thumbRightX < -2.f)
        //  thumbRightX = -2.f;
    }
    else
    {
        playerID = gamepad_->GetPlayerID();

        thumbRightX = gamepad_->GetRightThumbX();
        thumbRightY = gamepad_->GetRightThumbY();

        thumbLeftX = gamepad_->GetLeftThumbX();
        thumbLeftY = gamepad_->GetLeftThumbY();

        moveInputDirectionX += ((thumbLeftY * -currentAimUnitVecY) + (thumbLeftX * currentAimUnitVecX));
        moveInputDirectionZ += ((thumbLeftY * currentAimUnitVecX) + (thumbLeftX * currentAimUnitVecY));
    }
}

void PlayerBehavior::CameraMove(const int playerID, const float thumbRightX, const float thumbRightY) {

    /////////////////////////////////////////////////////////////////////////////
    // CAMERA ///////////////////////////////////////////////////////////////////

    CameraManager* pCameraManager = GetCameraManager();

    const Transform* pAttTrans = pCameraManager->getAttachmentTransform(playerID);
    vec3 cameraPosition = pCameraManager->getCameraOffsetVecter(playerID);
    const vec3 cameraAngle = pCameraManager->getCameraOffsetAngle(playerID);

    //////////////////
    // CAMERA POSITION

    float distanceXZ = sqrt(cameraPosition.x * cameraPosition.x + cameraPosition.z * cameraPosition.z);
    float radianX = thumbRightX * 0.02f;
    currentAimRadianX += radianX;

    // this position is circling aroung the player's object
    // it can be used as a view vector of where the palyer is aiming to
    currentAimUnitVecX = sin(currentAimRadianX);
    currentAimUnitVecY = cos(currentAimRadianX);
    float posX = currentAimUnitVecX * distanceXZ;
    float posZ = currentAimUnitVecY * distanceXZ;

    pCameraManager->setCameraOffsetVector(playerID, vec3(
        posX,
        cameraPosition.y,
        posZ
        ));

    //////////////////
    // CAMERA ROTATION

    // limitation of caemra's up and down angle
    float currentAimUnitVecX_Y = sin(currentAimRadianY);
    float currentAimUnitVecX_Z = cos(currentAimRadianY);
    if (currentAimUnitVecX_Y > PLAYER_BARREL_YAXIS_MAX) // upside
        currentAimRadianY = PLAYER_BARREL_YAXIS_MAX;
    else if (currentAimUnitVecX_Y < -PLAYER_BARREL_YAXIS_MIN) // downside
        currentAimRadianY = -PLAYER_BARREL_YAXIS_MIN;

    pCameraManager->setCameraOffsetRotation(playerID, vec3(
        -currentAimRadianX,
        currentAimRadianY,
        0.f
        ));

    // rotating the caemra angle
    float radianY = thumbRightY * 0.01f;
    currentAimRadianY += radianY;
    currentAimUnitVecXVertical = sin(currentAimRadianY);
    currentAimUnitVecYVertical = cos(currentAimRadianY);
}

void PlayerBehavior::TankMove(const int playerID, const float thumbLeftX, const float thumbLeftY, const float thumbRightX, const float thumbRightY) {

    // for the keyboard-input compatibility (to address a diagonal moving issue)
    btVector3 moveDir(-moveInputDirectionZ, 0.f, moveInputDirectionX);
    if (moveDir.getX() || moveDir.getY() || moveDir.getZ())
    {
        moveDir = moveDir.normalize();
        moveInputDirectionX = moveDir.getX();
        moveInputDirectionZ = moveDir.getZ();
    }

    // update players movement from gamepad
    Physics* phys = ObjectManager::GetObjectByID(GetParentID())->GetComponent<Physics>();
    phys->SetImpulse(
        moveInputDirectionX * moveVelocity,
        0.f,
        moveInputDirectionZ * moveVelocity
        );

    phys->getVelocity(); // have to call this to update the velocity

    BodyRotate();
    HeadRotate(thumbRightX, thumbRightY);

    // reset to zero. It should be on the most bottom line!
    moveInputDirectionX = 0;
    moveInputDirectionZ = 0;
}

void PlayerBehavior::BodyRotate()
{
    vec3 currViewEular;
    btQuaternion viewQuat = btQuaternion(currentAimRadianX - M_PI_2, 0.f, 0.f); // pHeadPhys->box->rigidbody->getOrientation();
    viewQuat.getEulerZYX(currViewEular.x, currViewEular.y, currViewEular.z);

    vec3 currBodyEular;
    Physics* pBodyPhys = ObjectManager::GetObjectByID(partBodyID)->GetComponent<Physics>();
    pBodyPhys->box->rigidbody->getOrientation().getEulerZYX(currBodyEular.x, currBodyEular.y, currBodyEular.z);

    currentBodyUnitVecX = cos(currBodyEular.y);
    currentBodyUnitVecY = -sin(currBodyEular.y);

    if (currBodyEular.x > 0)
    {
        currentBodyUnitVecX *= -1.f;
        currentBodyUnitVecY;
    }

    const float crossProduct = (currentAimUnitVecX * currentBodyUnitVecY) - (currentAimUnitVecY * currentBodyUnitVecX);

    pBodyPhys->box->rigidbody->setAngularVelocity(btVector3(0.f, crossProduct * PLAYER_BODY_ROTATION_SPEED, 0.f));
}

void PlayerBehavior::HeadRotate(const float thumbRightX, const float thumbRightY)
{
    Physics* pHeadPhys = ObjectManager::GetObjectByID(partHeadID)->GetComponent<Physics>();
    Physics* pBodyPhys = ObjectManager::GetObjectByID(partBodyID)->GetComponent<Physics>();
    const float currentAimUnitVecX_Y = sin(currentAimRadianY);
    const float currentAimUnitVecX_Z = cos(currentAimRadianY);

    // Head: horizontal
    {
        vec3 currBodyEular;
        pBodyPhys->box->rigidbody->getOrientation().getEulerZYX(currBodyEular.x, currBodyEular.y, currBodyEular.z);
        if (currBodyEular.x > 0)
            currBodyEular.y = M_PI_2 + (M_PI_2 - currBodyEular.y);

        // get the LAST constraint which is connected between the body and the head
        btHingeAccumulatedAngleConstraint* pAccuHingeBody =
            reinterpret_cast<btHingeAccumulatedAngleConstraint*>(
                pBodyPhys->box->rigidbody->getConstraintRef(
                    pBodyPhys->box->rigidbody->getNumConstraintRefs() - 1
                    ));
        const float currentHeadRadian = pAccuHingeBody->getHingeAngle() + M_PI_2 - currBodyEular.y;
        const float currentHeadUnitVecX = sin(currentHeadRadian);
        const float currentHeadUnitVecY = -cos(currentHeadRadian);

        const float crossProduct = (currentAimUnitVecX * currentHeadUnitVecY) - (currentAimUnitVecY * currentHeadUnitVecX);

        // makes the head rotate
        pAccuHingeBody->enableAngularMotor(true, -crossProduct * PLAYER_BARREL_ROTATION_SPEED_HORIZONTAL, 100000);
    }

    // Head: vertical
    {
        btHingeAccumulatedAngleConstraint* pAccuHingeHead = reinterpret_cast<btHingeAccumulatedAngleConstraint*>(pHeadPhys->box->rigidbody->getConstraintRef(0));
        const float currentHeadRadian = pAccuHingeHead->getHingeAngle() + M_PI_2;
        const float currentHeadUnitVecX = sin(currentHeadRadian);
        const float currentHeadUnitVecY = -cos(currentHeadRadian);

        const float crossProduct = (currentAimUnitVecX_Z * currentHeadUnitVecY) - (currentAimUnitVecX_Y * currentHeadUnitVecX);

        // makes the head rotate
        pAccuHingeHead->enableAngularMotor(true, -crossProduct * PLAYER_BARREL_ROTATION_SPEED_VERTCIAL, 100000);
    }

    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    // it keeps the radian between 0 - 360 degree
    currentAimRadianX = convert2PositiveRadian(currentAimRadianX);
    currentAimRadianY = convert2PositiveRadian(currentAimRadianY);
}

int PlayerBehavior::GetPlayerID()
{
  return m_playerID;
}

void PlayerBehavior::SetPowerUpTimer()
{
    powerUpTimer = 60 * 10;
    powerUp = true;
}

void PlayerBehavior::RecoverHP()
{
    ParticleEmitter* pe = ObjectManager::GetObjectByID(partBodyIDParticle)->GetComponent<ParticleEmitter>(1);

    pe->SetActive(true);
    HP = 5;
    recover = true;
}

void PlayerBehavior::setTimeStamp(float stamp)
{
    timeStamp_ = stamp;
}

float PlayerBehavior::getTimeStamp()
{
    return timeStamp_;
}

void PlayerBehavior::SetTwinkling(const int _isTwinkling)
{
    // ignore if it indicates the same state
    if (_isTwinkling == isTwinkling)
        return;

    isTwinkling = _isTwinkling;
    // init when true
    if (_isTwinkling == 1)
    {
        colorCurrentOffset.x = ((rand() % 10) + 0) / 5.f;
        colorCurrentOffset.y = ((rand() % 10) + 0) / 5.f;
        colorCurrentOffset.z = ((rand() % 10) + 0) / 5.f;
    }
    else if (_isTwinkling == 2)
    {

    }
    // reset to the original state
    else
    {
        colorCurrent = colorOrigin;

        // set to the origin color
        GameObject* thisObject = ObjectManager::GetObjectByID(GetParentID());
        for (auto childObj : thisObject->GetChildren())
        {
            ModelComponent* pModel = ObjectManager::GetObjectByID(childObj)->GetComponent<ModelComponent>();
            if (pModel != nullptr)
            {
                ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
                    colorOrigin.x,
                    colorOrigin.y,
                    colorOrigin.z);
            }
        }
    }
}

void PlayerBehavior::PowerUp()
{
    if (powerUp)
    {
        if (isInvincible())
            PlayerAttController::SetTwinkling(gamepad_->GetPlayerID(), 1);

        if (slow)
            PlayerAttController::SetTwinkling(gamepad_->GetPlayerID(), 2);

        if (fast)
        {
            ParticleEmitter* pe = ObjectManager::GetObjectByID(partBodyIDParticle)->GetComponent<ParticleEmitter>();
            pe->SetActive(true);
        }


        //////////////////////////////////////////////////////////////
        //PlayerAttController::SetTwinkling(gamepad_->GetPlayerID(), 2);

        --powerUpTimer;
        if (powerUpTimer < 0)
        {
            //Reset
            slow = false;
            fast = false;
            ParticleEmitter* pe = ObjectManager::GetObjectByID(partBodyIDParticle)->GetComponent<ParticleEmitter>();
            pe->SetActive(false);
            PlayerAttController::ResetAllAttributes(gamepad_->GetPlayerID());
            vincible();
            powerUp = false;
            PlayerAttController::SetTwinkling(gamepad_->GetPlayerID(), 0);
        }
    }
}

void PlayerBehavior::Twinkling(const float dt)
{
    if (isTwinkling == 0)
        return;

    static float savedTime = 0;
    savedTime += (dt * 10);

    if (isTwinkling == 1)
    {
        colorCurrent.x = std::cos(savedTime);
        colorCurrent.y = std::sin(-savedTime);
        colorCurrent.z = std::tan(savedTime) + 0.3f;

    }
    else// if (isTwinkling == 2)
    {
        colorCurrent.x = std::cos(savedTime) * 0.4f;
        colorCurrent.y = 0;
        colorCurrent.z = std::cos(savedTime) * 0.80f;

        if (colorCurrent.x < 0)
            colorCurrent.x *= -1;

        if (colorCurrent.z < 0)
            colorCurrent.z *= -1;

        //colorCurrent.x = 0.4f;
        //colorCurrent.z = 0.8f;
    }

    GameObject* thisObject = ObjectManager::GetObjectByID(GetParentID());
    for (auto childObj : thisObject->GetChildren())
    {
        GameObject* partObj = ObjectManager::GetObjectByID(childObj);
        if (partObj->GetName() != "player_accent_left" && partObj->GetName() != "player_accent_right" && partObj->GetName() != "player_accent_back")
        {
            ModelComponent* pModel = ObjectManager::GetObjectByID(childObj)->GetComponent<ModelComponent>();
            if (pModel != nullptr)
            {
                ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
                    colorCurrent.x * (float)HP / 5.0f,
                    colorCurrent.y * (float)HP / 5.0f,
                    colorCurrent.z * (float)HP / 5.0f);
            }
        }
    }
}

void PlayerBehavior::Vibration(const float dt)
{
    // no need to vibration
    if (vibrateDuration == 0.f)
        return;

    if (currentVibrateDuration < vibrateDuration)
        currentVibrateDuration += dt;
    else
    {
        gamepad_->Vibrate(0.f, 0.f); // cease the vibration
        vibrateDuration = 0.f;
    }
}