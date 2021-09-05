/******************************************************************************
  file: ModelComponent.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#include "Component.h"
#include "..//..//PARender/inc/PARenderAPI/IModel.h"  //  InstanceID

enum ModelType {
    MT_INVALID = -1,
    MT_CUBE_METALGRID,
    MT_CUBE_DARK,
    MT_WALL_LARGE,
    MT_WALL_BRICK,
    MT_FLOOR_CONCRETE,
    MT_FLAG,
    MT_GAUGE,
    MT_TUTORIAL,
    MT_DIGIPEN_LOGO,
    MT_MONO_COLOR_CUBE,
    MT_MONO_COLOR_SPHERE,
    MT_SPHERE,
    MT_FUEL_DEPOT,

    // None below here//
    MT_NUM,
};

class ModelComponent : public Component {
public:
    ModelComponent(int managerIdx, ComponentManager* manager);
    ~ModelComponent();

    ModelComponent* Create(int objectID, ModelType mt);
    void Destroy() override;

    ModelType GetModeltype() { return modelType_; }

    Rendering::IModel::InstanceID GetInstance() { return instance_; }
private:
    ModelType modelType_;
    Rendering::IModel::InstanceID instance_;

    friend class ModelManager;
};
