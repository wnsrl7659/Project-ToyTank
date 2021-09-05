/******************************************************************************
  file: ModelComponent.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "ModelComponent.h"
#include "ModelManager.h"

ModelComponent::ModelComponent(int managerIdx, ComponentManager* manager)
    : Component(managerIdx, manager), modelType_(MT_INVALID), instance_() {
}

ModelComponent::~ModelComponent() {
}

ModelComponent* ModelComponent::Create(int objectID, ModelType mt)
{
    SetParent(objectID);
    modelType_ = mt;
    instance_ = ModelManager::models_[modelType_]->AddInstance(nullptr);
    ModelManager::models_[modelType_]->Renderable(instance_, true);
    if (mt == MT_WALL_LARGE)
    {
        ModelManager::models_[modelType_]->Renderable(instance_, false);
    }
    return this;
}

void ModelComponent::Destroy() {
    ModelManager::models_[modelType_]->RemoveInstance(instance_);
    modelType_ = MT_INVALID;
    SetParent(-1);
}
