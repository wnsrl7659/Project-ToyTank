/******************************************************************************
  file: GameObject.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#include <array>
#include <string>
#include <unordered_set>
#include "Component.h"
#include "ComponentManager.h"
#define MAX_COMPONENT 16

class GameObject : public Component {
public:
  GameObject(int managerIdx);
  ~GameObject();

  ////////////////////////////////////////////
  //  Component Operations                  //
  void AddComponent(Component* component);
  template<typename COMPONENT>
  void RemoveComponent();
  template <typename COMPONENT>
  COMPONENT* GetComponent(unsigned number = 0) const;
  ////////////////////////////////////////////

  const int& GetID() const { return objectID_; }
  const std::string& GetName() const { return name_; }
  void SetName(std::string name)  {  name_ = name; }
  std::unordered_set<int>& GetChildren() { return childIDs_; }
  void Destroy() override;

private:
  int objectID_;  // unique ID given by the ObjectManager
  std::string name_; 
  unsigned numComponents; // number of components owned by the GameObject
  std::array<Component*, MAX_COMPONENT> components_; // List of all components associated with the GameObject
  std::unordered_set<int> childIDs_; // List of components that are gameobjects
  int ttl; // frames left before object manager destroys this object;
  friend class ObjectManager;
};

template<typename COMPONENT>
inline void GameObject::RemoveComponent() {
  for (unsigned i = 0; i < numComponents ; ++i) {
    COMPONENT* toRemove = dynamic_cast<COMPONENT*>(components_[i]);
    if (toRemove) {
      components_[i]->GetManager()->Deallocate(components_[i]->compID);
      components_[i] = components_[numComponents - 1];
      components_[numComponents - 1] = nullptr;
      --numComponents;
    }
  }
}

template<typename COMPONENT>
inline COMPONENT* GameObject::GetComponent(unsigned number) const
{
  for (unsigned i = 0; i < numComponents; ++i) {
    COMPONENT* result = dynamic_cast<COMPONENT*>(components_[i]);
    if (result) {
      if (number == 0) {
        return result;
      }
      else {
        --number;
      }
    }
  }
  return nullptr;
}
