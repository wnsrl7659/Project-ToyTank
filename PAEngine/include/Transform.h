/******************************************************************************
  file: Transform.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#include <vector>
#include "Component.h"

class BoxCollider;

class Physics : public Component
{
public:
  Physics(int managerIdx, ComponentManager* manager);
  ~Physics();
  Physics* Create(int parentID, float xScale = 1.0f, float yScale = 1.0f, float zScale = 1.0f, float mass = 0.0f,
    float xPos = 0.0f, float yPos = 0.0f, float zPos = 0.0f);
  void Destroy() override;

  // setter
  void SetImpulse(float x, float y, float z);
  inline void SetMaxVelocity(float vel) { maxVelocity = vel; }
  void Teleport(float x, float y, float z);
  void SetMass(float mass);
  void SetPosition(float x, float y, float z);
  void SetPositionY(float y);

  // getter
  std::vector<float>& GetMatrix();
  float* const getVelocity();
  const float GetCurrentVelocity() const;
  const float GetMaxVelocity() const { return maxVelocity; }

  BoxCollider* box;
private:

  //  TODO: Seperate impulses from Transform into their own component
  float impulseX;
  float impulseY;
  float impulseZ;
  float maxVelocity;

  friend class PhysicsManager;
  friend class WallState;
};