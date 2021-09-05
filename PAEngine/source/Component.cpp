/******************************************************************************
  file: Component.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "Component.h"

Component::Component(int managerIdx, ComponentManager* manager) 
  : parentID_(-1), compID(managerIdx), manager_(manager) {
}

Component::~Component() {
}

void Component::SetParent(int parentID) {
  parentID_ = parentID;
}

int const& Component::GetParent() const { 
  return parentID_; 
}

ComponentManager* Component::GetManager() const
{
  return manager_;
}

const int& Component::GetID() const
{
  return compID;
}
