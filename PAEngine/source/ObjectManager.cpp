/******************************************************************************
  file: ObjectManager.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "ObjectManager.h"
#include "ComponentManager.h"
#include "TransformManager.h"
#include "ChildTransformManager.h"
#include "ModelManager.h"
#include "BehaviorManager.h"
#include "ParticleEmitterManager.h"
#include "PAGuiWindow.h"
#include "PAGuiWidgets.h"
#include "ObjectFactory.h"


ObjectManager* ObjectManager::instance_ = nullptr;
static const std::string invalidObjectName = "Invalid Object";

ObjectManager* ObjectManager::GetInstance() {
  if (instance_ == nullptr) {
    instance_ = new ObjectManager();
  }
  return instance_;
}

ObjectManager::ObjectManager() 
  : componentManagers_(){
  componentManagers_.push_back(PhysicsManager::GetInstance());
  componentManagers_.push_back(ModelManager::GetInstance());
  componentManagers_.push_back(BehaviorManager::GetInstance());
  componentManagers_.push_back(TransformManager::GetInstance());
  componentManagers_.push_back(ParticleEmitterManager::GetInstance());
}

ObjectManager::~ObjectManager() {
  instance_ = nullptr;
  for (ComponentManager* CM : componentManagers_) {
    delete CM;
  }
}

bool ObjectManager::IsValidID(int objectID) {
  return objectID >= 0 && objectID < OM_MAX_OBJECTS ;
}

void ObjectManager::DestroyObject(int objectID, int delay) {
  if (IsValidID(objectID)) {
    if (instance_->allocated_[objectID])
    {
      GameObject* toDestroy = reinterpret_cast<GameObject*>(instance_->Get(objectID));
      toDestroy->ttl = delay;
    }
  }
}

GameObject* ObjectManager::GetObjectByID(int objectID) {
  if (IsValidID(objectID) && instance_->allocated_[objectID]) {
    return reinterpret_cast<GameObject*>(instance_->Get(objectID));
  }
  return nullptr;
}

std::vector<GameObject*> ObjectManager::GetObjectsByName(const std::string& name) {
  std::vector<GameObject*> result;
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    GameObject* object = reinterpret_cast<GameObject*>(instance_->Get(i));
    if (instance_->allocated_[i] && object->name_ == name) {
      result.push_back(object);
    }
  }
  return result;
}

std::unordered_set<int> ObjectManager::GetChildObjects(int objectID)
{
  if (IsValidID(objectID) && instance_->allocated_[objectID]) {
    GameObject* obj = dynamic_cast<GameObject*>(instance_->components_[objectID]);
    return obj->childIDs_;
  }

  return std::unordered_set<int>();
}

void ObjectManager::MakeParent(int childID, int parentID) {
  if (IsValidID(childID) && IsValidID(parentID)) {
    instance_->components_[childID]->SetParent(parentID);
    dynamic_cast<GameObject*>(instance_->components_[parentID])->AddComponent(instance_->components_[childID]);
  }
}

void ObjectManager::RemoveParent(int childID) {
  if (IsValidID(childID)) {
    GameObject* child = reinterpret_cast<GameObject*>(instance_->Get(childID));
    GameObject* parent = reinterpret_cast<GameObject*>(instance_->Get(child->parentID_));
    child->parentID_ = -1;
  }
}

void ObjectManager::ClearObjects() {
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    GameObject* currentObj = reinterpret_cast<GameObject*>(instance_->components_[i]);
;
    currentObj->Destroy();
    instance_->allocated_[i] = false;
  }
}

void ObjectManager::Init(){
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    components_[i] = new GameObject(i);
  }
  for (ComponentManager*& CM : componentManagers_) {
    CM->Init();
  }
}

void ObjectManager::Update(float dt){
  // Update other component managers, move this to a different funtion
  for (ComponentManager*& CM : componentManagers_) {
    CM->Update(dt);
  }

  // update GameObject Components
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    GameObject* currentObj = reinterpret_cast<GameObject*>(components_[i]);
    if (currentObj->ttl == 0) {
      // TODO: Deallocate the components

      //  clear objectData
      currentObj->Destroy();
      instance_->allocated_[i] = false;
    }
    else if (currentObj->ttl > 0) {
      currentObj->ttl--;
    }
  }
}

void ObjectManager::Draw() {
  for (ComponentManager*& CM : componentManagers_) {
    CM->Draw();
  }
}

void ObjectManager::Shutdown(){
  ObjectManager::ClearObjects();
  for (ComponentManager*& CM : componentManagers_) {
    CM->Shutdown();
  }
}
