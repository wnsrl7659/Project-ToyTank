/******************************************************************************
  file: Bahavior.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "Behavior.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "BehaviorState.h"

Behavior::Behavior()
  : behavType_(BT_INVALID), currentState_(-1), behavStates_(), timer_(0.0f) {
}

Behavior::~Behavior() {
  for (unsigned i = 0; i < behavStates_.size(); ++i) {
    delete behavStates_[i];
    behavStates_[i] = nullptr;
  }
}

Behavior* Behavior::Init(int parentID_, BehaviorType bType) {
  GameObject* thisObject = ObjectManager::GetObjectByID(parentID_);
  //  get the behavior at the parents index
  Behavior* behavior = thisObject->GetComponent<Behavior>();
  //  build base class and return
  behavior->SetParent(parentID_);
  behavior->behavType_ = bType;
  return behavior;
}

void Behavior::Update(float dt) {
  //  calling the current states update function for the derived class
  if (currentState_ != -1) {
    behavStates_[currentState_]->Update(dt);
    timer_ += dt;
  }
}

void Behavior::Shutdown() {
  if (currentState_ != -1) {
    behavStates_[currentState_]->Shutdown();
  }

  currentState_ = -1;
}

void Behavior::ChangeState(unsigned nextState) {
  if (nextState != currentState_) {
    if (currentState_ != - 1) {
      behavStates_[currentState_]->Shutdown();
    }
    currentState_ = nextState;
    if (currentState_ != -1) {
      behavStates_[currentState_]->Init();
    }
  }
}

float& Behavior::GetTimer() {
  return timer_;
}

float Behavior::GetTimerConst() const
{
  return timer_;
}

int Behavior::GetParentID() const {
  return GetParent();
}

BehaviorType Behavior::GetType() const
{
  return behavType_;
}

void Behavior::AddBehaviorState(BehaviorState* newState) {
  if (newState) {
    behavStates_.push_back(newState);
  }
}

