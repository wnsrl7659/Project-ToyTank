/******************************************************************************
  file: BehaviorManager.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "BehaviorManager.h"
#include "PlayerBehavior.h"
#include "AIBehavior.h"
#include "GameObject.h"
#include "ObjectManager.h"
#include "FlagBehavior.h"
#include "BulletBehavior.h"
#include "GaugeBehavior.h"
#include "AstarBehavior.h"
#include "PowerUpsBehavior.h"
#include "FuelDepotBehavior.h"
BehaviorManager* BehaviorManager::instance_ = nullptr;
std::array<BehaviorType, OM_MAX_OBJECTS> BehaviorManager::behaviorTypes_ = std::array<BehaviorType, OM_MAX_OBJECTS>();
std::array<Component*, OM_MAX_OBJECTS>* BehaviorManager::behavComponents_ = nullptr;
std::array<bool, OM_MAX_OBJECTS>* BehaviorManager::allocatedBehaviors_ = nullptr;

BehaviorManager::BehaviorManager() 
  : ComponentManager() {
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    components_[i] = nullptr;
    behaviorTypes_[i] = BT_INVALID;
  }
  behavComponents_ = &components_;
  allocatedBehaviors_ = &allocated_;
}

BehaviorManager* BehaviorManager::GetInstance()
{
  if (!instance_) {
    instance_ = new BehaviorManager();
  }
  return instance_;
}

BehaviorManager::~BehaviorManager() {

}

void BehaviorManager::Init() {
  //  this is where we would pre-allocate known behavior types
  //  example:  4 PlayerBehaviors
}

void BehaviorManager::Update(float dt) {
  //  updates all the behaviors
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    if (allocated_[i]) {
      reinterpret_cast<Behavior*>(components_[i])->Update(dt);
    }
  }
}

void BehaviorManager::Draw() {
}

void BehaviorManager::Shutdown() {
  //  deleting allocated behaviors and setting their types to INVALID
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    delete components_[i];
    components_[i] = nullptr;
    allocated_[i] = false;
  }
}

static Behavior* newBehaviorOfType(BehaviorType bt) {
  switch (bt)
  {
  case BT_PLAYER:
    return new PlayerBehavior();
    break;
  case BT_AI:
    return new AIBehavior();
    break;
  case BT_FLAG:
    return new FlagBehavior();
    break;
  case BT_BULLET:
    return new BulletBehavior();
  case BT_GAUGE:
    return new GaugeBehavior();
  case BT_ASTAR:
    return new AstarBehavior();
  case BT_POWERUP:
    return new PowerUpsBehavior();
  case BT_FUELDEPOT:
    return new FuelDepotBehavior();
  default:
    return nullptr;
    break;
  }
}

Behavior* BehaviorManager::ReallocateBehaviorType(int behavIdx, BehaviorType behaviorType, int parentID) {
  Behavior** behavior = reinterpret_cast<Behavior**>(&(instance_->components_[behavIdx]));

  // check if memory allocated for behavior
  if (*behavior) {
    // memory allocated, check if the datastructure is correct
    if ((*behavior)->GetType() != behaviorType) {
      //incorrect datastructure
      delete* behavior;
      *behavior = nullptr;
    }
  }

  // check if we need a new behavior, old behavior can be 
  // deleted in prev if statement
  if (*behavior == nullptr) {
    *behavior = newBehaviorOfType(behaviorType);
  }

  // initialize values
  (*behavComponents_)[behavIdx] = *behavior;
  (*behavior)->compID = behavIdx;
  (*behavior)->manager_ = instance_;
  (*allocatedBehaviors_)[behavIdx] = true;

  return *behavior;
}
