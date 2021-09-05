/******************************************************************************
  file: TransformManager.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#include "ComponentManager.h"

//////////////////////////////////////////////////////////////////////////////
//  Move to Component base class file and Transform
#include "Physics.h"

class PhysicsManager : public ComponentManager {
public:
  static PhysicsManager* GetInstance();
	~PhysicsManager();

  void Init() override;  //  allocate mem for transform
	void Update(float dt) override;  //  update transform components
  void Draw() override;
  void Shutdown() override;  //  frees memory from transforms
private:
  static PhysicsManager* instance_;
	PhysicsManager();
};