/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/
#pragma once
#include "Behavior.h"
#include "InputDevices.h" // gamepad
/***************************************************************************/
/*  Forward declarations                                                   */
class ChangeT; //  movement controller

/*  Behavior used by Player objects that are being   *
 *  controlled by people using a gamepad.            */
class PlayerBehavior : public Behavior {
public:
  // States used in the state machine
  enum PlayerBehaviorState { // change anything you need to, might need to move to public
    PBS_INVALID = -1,
    PBS_PLAYING,
    PBS_RAPID_FIRE,
    PBS_DEAD,
    PBS_BIGSHOT
  };

  //constructor should be used to add functions to the base class's function lists
  PlayerBehavior();
  ~PlayerBehavior();
  
  //  use Create and Destroy to initialize and clear values specific to each player
  void Create(PAEngine::Gamepad* gamepad, int parentID);
  void Destroy();

  inline void SetPartIDs(const int _headID, const int _bodyID, const int bodyParticleID){
    partHeadID = _headID;  partBodyID = _bodyID;  partBodyIDParticle = bodyParticleID;
  }
  inline void AddCurrAimRadianX(float _currAimRadX) { currentAimRadianX += _currAimRadX; }
  inline void AddCurrAimRadianY(float _currAimRadY) { currentAimRadianY += _currAimRadY; }
  inline const float GetCurrAimUnitVecX() const { return currentAimUnitVecX; }
  inline const float GetCurrAimUnitVecY() const { return currentAimUnitVecY; }
  inline PAEngine::Gamepad* GetGamepad() const { return gamepad_; }
  inline void ClearGamepad(void) { gamepad_ = nullptr; } // only for DeviceDisconnected

  void SetMaxVelocity(const float maxVel);
  const float GetMaxVelocity();

  void Shoot(const float bulletSize = 0.5f, const float bulletVelcocity = 30.f, bool explode = false, bool damagePlayer = true, const float recoil = 75.f);
  void StartGamepadVibration(const float duration, const float normalizedLeftSpeed, const float normalizedRightSpeed);

  float initialX;
  float initialZ;
  int HP;
  bool hasFlag = false;
  float moveVelocity;
  float weaponeFirerate;
  float bigShotFireRate;
  vec3 colorOrigin;

  float thumbLeftX = 0;
  float thumbLeftY = 0;
  float thumbRightX = 0;
  float thumbRightY = 0;

  float moveInputDirectionX_keyboard;
  float moveInputDirectionZ_keyboard;
  float moveInputDirectionZ; // forward backward
  float moveInputDirectionX; // left right

  int GetPlayerID();
  void SetPowerUpTimer();
  void SetTwinkling(const int _isTwinkling);
  void RecoverHP();
  void invincible() { invincibility = true; }
  void vincible() { invincibility = false; }
  bool isInvincible() { return invincibility; }
  void setTimeStamp(float stamp);
  float getTimeStamp();

  bool slow = false;
  bool fast = false;
  bool recover = false;
  int recoverTimer = 0;
  float respawnTimer;
  float fuel = 100;
  float fuelTileCost = 2.0f; // how much each tile costs
  int shotBy = -1;
  bool filling = false;
  int partBodyIDParticle;

private:

  //  private data members
  int m_playerID;
  PAEngine::Gamepad* gamepad_;
  ChangeT* forces_; //  used to move players transform


  float time_;
  float timeStamp_ = 0.0f;

  int partHeadID;
  int partBodyID;

  // Aim(Camera) : 0.0 - 1.0
  float currentAimUnitVecX;
  float currentAimUnitVecY;

  float currentAimUnitVecXVertical;
  float currentAimUnitVecYVertical;

  // Aim(Camera) : 0.0 - M_PI_DOUBLE (360)
  float currentAimRadianX;
  float currentAimRadianY;
  float currentBodyUnitVecX;
  float currentBodyUnitVecY;

  friend class PlayerBehavStateDefault;
  friend class PlayerBehavStateRapidFire;
  friend class PlayerBehavStateDead;
  friend class PlayerBigShot;

  // functions in PlayerBehavStateDefault::Update
  void UpdateInput(int& playerID, float& thumbLeftX, float& thumbLeftY, float& thumbRightX, float& thumbRightY);
  void CameraMove(const int playerID, const float thumbRightX, const float thumbRightY);
  void TankMove(const int playerID, const float thumbLeftX, const float thumbLeftY, const float thumbRightX, const float thumbRightY);
  void BodyRotate();
  void HeadRotate(const float thumbRightX, const float thumbRightY);
  void Twinkling(const float dt);
  void Vibration(const float dt);
  void PowerUp();

  int powerUpTimer = 0;
  bool powerUp = false;
  bool invincibility = false;
  int isTwinkling; // twinkling when the player is invincible
  bool timeForTWK;

  vec3 colorCurrent;
  vec3 colorCurrentOffset;
  float vibrateDuration;        // total frame duration of vibration until it stops
  float currentVibrateDuration; // current frame duaration of the vibration

  void playerUpdate(const float dt);
};

void BulletCollidesWithPlayer_CB(unsigned myID, unsigned otherID, const std::string& bullet);
void BulletsCollidesWithPlayer_CB(unsigned myID, unsigned otherID, const std::string& bullet);
void GroundCollidesWithPlayer_CB(unsigned myID, unsigned otherID, const std::string& ground);