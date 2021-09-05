/******************************************************************************
  file: TransformManager.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/

#include "pch.h"
#include "TransformManager.h"
#include "Transform.h"
#include "ComponentManager.h"
#include "Physics.h"

PhysicsManager* PhysicsManager::instance_ = nullptr;

PhysicsManager::PhysicsManager()
  : ComponentManager() {

}

PhysicsManager* PhysicsManager::GetInstance() {
  if (!instance_) {
    instance_ = new PhysicsManager();
  }
  return instance_;
}

PhysicsManager::~PhysicsManager() {

}

void PhysicsManager::Init() {
  //  allocate memory for transforms on base classes array
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    components_[i] = new Physics(i, this);
  }
}

void PhysicsManager::Update(float dt) {
  //  use the transforms impulse members to change the transform

  //  iterate through components
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    if (allocated_[i]) {
      Component* c = components_[i];
      if (c->GetParent() != -1)
      {
        Physics* t = dynamic_cast<Physics*>(c);

        // addImpulse to BoxCollider
        if (t->maxVelocity)
        {
          if ((t->GetCurrentVelocity() < t->GetMaxVelocity()))
            t->box->addImpulse((btVector3(t->impulseX, t->impulseY, t->impulseZ)));
        }
        else
          t->box->addImpulse((btVector3(t->impulseX, t->impulseY, t->impulseZ)));

        // reset
        t->impulseX = 0.0f;
        t->impulseY = 0.0f;
        t->impulseZ = 0.0f;
      }
    }
  }
}

void PhysicsManager::Draw() {
  //  nothing here yet, maybe debug draw?
}

void PhysicsManager::Shutdown() {
  //  delete memory allocated in init
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    delete components_[i];
    components_[i] = nullptr;
    allocated_[i] = false;
  }
}
