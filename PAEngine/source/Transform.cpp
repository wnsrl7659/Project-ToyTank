/******************************************************************************
  file: Transform.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "Transform.h"
#include "Physics.h" //  BoxCollider

Physics::Physics(int managerIdx, ComponentManager* manager)
  : Component(managerIdx, manager), impulseX(0.f), impulseY(0.f), impulseZ(0.f), maxVelocity(0.f), box(nullptr) {
}

Physics::~Physics() {
}

Physics* Physics::Create(int parentID, float xScale, float yScale, float zScale, float mass, float xPos, float yPos, float zPos) {
  SetParent(parentID);
  box = new BoxCollider(parentID, xScale, yScale, zScale, mass, xPos, yPos, zPos);

  return this;
}

void Physics::Destroy() {
  delete box;
}

void Physics::SetImpulse(float x, float y, float z) {
  impulseX += x; impulseY += y; impulseZ += z;
}

void Physics::Teleport(float x, float y, float z) {
  box->translate(btVector3(x, y, z));
}

void Physics::SetMass(float mass)
{
  box->setMass(mass);
}

void Physics::SetPosition(float x, float y, float z)
{
    box->setPosition(btVector3(x, y, z));
}

void Physics::SetPositionY(float y)
{
    btVector3 pos = box->getPosition();
    box->setPosition(btVector3(pos.x(), y, pos.z()));
}



std::vector<float>& Physics::GetMatrix() {
  return box->getTransform();
}

float* const Physics::getVelocity()
{
  return box->getVelocity();
}

const float Physics::GetCurrentVelocity() const
{
  return box->getLinearVelocity().norm();
}