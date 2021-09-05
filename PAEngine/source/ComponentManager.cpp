/******************************************************************************
  file: ComponentManager.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/

#include "pch.h"
#include "ComponentManager.h"
#include "Component.h"

ComponentManager::ComponentManager() {
  for (bool& b : allocated_) {
    b = false;
  }
}
ComponentManager::~ComponentManager() {}

int ComponentManager::Allocate()
{
  for (int i = 0; i < allocated_.size(); ++i) {
    if (!allocated_[i]) {
      allocated_[i] = true;
      return i;
    }
  }
  return -1;
}

void ComponentManager::Deallocate(int componentIndex) {
  if (allocated_[componentIndex]) {
    components_[componentIndex]->Destroy();
    allocated_[componentIndex] = false;
  }
  else {
    std::cout << "doubleDelet\n";
  }
}

Component* ComponentManager::Get(int componentIndex)
{
  if (componentIndex != -1) {
    return components_[componentIndex];
  }
  return nullptr;
}
