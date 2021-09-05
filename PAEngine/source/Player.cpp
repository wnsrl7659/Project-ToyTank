/******************************************************************************
  file: Player.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "Player.h"
#include "ObjectManager.h"
#include "PlayerBehavior.h"
#include "ObjectFactory.h"
#include "PlayerAttributesController.h"

Player::Player()
  :  inputType_(PIT_INVALID), objectID_(-1), gamepad_(nullptr), behav_(nullptr) {
}

Player::~Player() {
}

void Player::Init(const int ID) {
  switch (ID)
  {
  case 0:
    InitPos_ = vec3(75.0f, 15.0f, 75.0f);
    color = vec3(1, 0, 0);
    break;
  case 1:
    InitPos_ = vec3(-75.0f, 15.0f, 75.0f);
    color = vec3(0, 0, 1);
    break;
  case 2:
    InitPos_ = vec3(-75.0f, 15.0f, -75.0f);
    color = vec3(1, 1, 0);
    break; 
  case 3:
    InitPos_ = vec3(75.0f, 5.0f, -75.0f);
    color = vec3(0, 1, 0);
    break;
  }
  switch (inputType_)
  {
  case PIT_INVALID:
    break;
  case PIT_GAMEPAD_0:
  case PIT_GAMEPAD_1:
  case PIT_GAMEPAD_2:
  case PIT_GAMEPAD_3:
    objectID_ = ObjectFactory::CreatePlayer(gamepad_, InitPos_.x, InitPos_.y, InitPos_.z);
    PlayerAttController::SetColor(gamepad_->GetPlayerID(), color);
    //ObjectFactory::CreateGauge(gamepad_->GetPlayerID(), objectID_);
    break;
  case PIT_AI_0:
    // initialize AI players
    //objectID_ = ObjectFactory::CreateAI(ID, InitPos_.x, InitPos_.y, InitPos_.z);
    //ObjectFactory::CreateGauge(ID, objectID_);
    break;
  default:
    break;
  }
}

void Player::Shutdown() {
  inputType_ = PIT_INVALID;
}

void Player::RecreateObject(float _posX, float _posY, float _posZ)
{
  // re-initializes pointers
  objectID_ = ObjectFactory::CreatePlayer(gamepad_, _posX, _posY, _posZ);
  PlayerAttController::SetColor(gamepad_->GetPlayerID(), color);

  behav_ = ObjectManager::GetObjectByID(objectID_)->GetComponent<Behavior>();
}

PlayerInputType Player::GetInputType() {
  return inputType_;
}

void Player::RecieveInput() {
  
}
