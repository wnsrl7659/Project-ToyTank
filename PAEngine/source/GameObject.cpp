/******************************************************************************
  file: GameObject.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "GameObject.h"
#include "ObjectManager.h"
#include "PAGuiWindow.h"
#include "PAGuiWidgets.h"

GameObject::GameObject(int managerIdx = -1)
  :  Component(managerIdx, ObjectManager::GetInstance()), objectID_(-1), name_("Uninitialized Object"), numComponents(0), components_(), ttl(-1) {
  for (unsigned i = 0; i < MAX_COMPONENT; ++i) {
    components_[i] = nullptr;
  }
  objectID_ = managerIdx;
}

GameObject::~GameObject() { 
}

void GameObject::AddComponent(Component* component) {
  if (component) {
    if (numComponents < MAX_COMPONENT) {
      components_[numComponents] = component;
      component->SetParent(this->GetID());
      ++numComponents;
      if (dynamic_cast<GameObject*>(component)) {
        childIDs_.insert(component->GetID());
      }
    }
  }
}

void GameObject::Destroy() {
  //////////////////////////////////
  tn_object->DeleteWidget(objectID_);
  //////////////////////////////////

  //  remove this id from parents children
  if (parentID_ != -1) {
    GameObject* parent = ObjectManager::GetObjectByID(parentID_);
  }
  //  clear data
  name_ = "InvalidObject";
  parentID_ = -1;
  for (unsigned i = 0; i < numComponents; numComponents--) {
    ComponentManager* compManager = components_[numComponents - 1]->GetManager();
    if (compManager) {
      compManager->Deallocate(components_[numComponents - 1]->GetID());
    }
    components_[numComponents - 1] = nullptr;
  }
  ttl = -1;
}
