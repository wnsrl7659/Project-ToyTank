/******************************************************************************
  file: ChildTransformManager.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#include "ComponentManager.h"

class TransformManager : public ComponentManager {
public:
  static TransformManager* GetInstance();
  ~TransformManager();

  void Init() override;  //  allocate mem for transform
  void Update(float dt) override;  //  update transform components
  void Draw() override;
  void Shutdown() override;  //  frees memory from transforms
private:
  static TransformManager* instance_; // singleton isntance
  TransformManager();
};