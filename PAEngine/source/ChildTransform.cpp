/******************************************************************************
  file: ChildTransform.cpp
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#include "pch.h"
#include "ChildTransform.h"
#include "ChildTransformManager.h"

Transform::Transform(int managerIdx, ComponentManager* manager)
  : Component(managerIdx, manager) {
  relativeMatrix_ = { 1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, 0,
                     0, 0, 0, 1 };

  transformMatrix_ = { 1, 0, 0, 0,
                       0, 1, 0, 0,
                       0, 0, 1, 0,
                       0, 0, 0, 1 };
}

Transform::~Transform() {
}

Transform* Transform::Create(int objectID , float scaleX, float scaleY, float scaleZ, float relativeX, float relativeY, float relativeZ) {
  relativeMatrix_ = { scaleX, 0, 0, relativeX,
                      0, scaleY, 0, relativeY,
                      0, 0, scaleZ, relativeZ,
                      0,    0,    0,    1      };
  SetParent(objectID);

  return this;
}

void Transform::Destroy() {
  relativeMatrix_ =  { 1, 0, 0, 0,
                       0, 1, 0, 0,
                       0, 0, 1, 0,
                       0, 0, 0, 1 };
  
  transformMatrix_ = { 1, 0, 0, 0,
                       0, 1, 0, 0,
                       0, 0, 1, 0,
                       0, 0, 0, 1 };
}

const float* Transform::GetTransform() const
{
  return transformMatrix_.data();
}

float Transform::GetPositionX() const {
  return transformMatrix_[12];
}

float Transform::GetPositionY() const {
  return transformMatrix_[13];
}

float Transform::GetPositionZ() const {
  return transformMatrix_[14];
}

void Transform::GetPosition(float* position) const
{
  position[0] = GetPositionX();
  position[1] = GetPositionY();
  position[2] = GetPositionZ();
}

void Transform::SetRelativePosition(const float _x, const float _y, const float _z)
{
  relativeMatrix_[3] = _x;
  relativeMatrix_[7] = _y;
  relativeMatrix_[11] = _z;
}

void Transform::SetRelativeRotationX(const float _radian)
{
  float cos = cosf(_radian);
  float sin = sinf(_radian);

  std::array<float, 16> rotMatix = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };

  rotMatix[5] = cos * relativeMatrix_[5];
  rotMatix[6] = -sin;
  rotMatix[9] = sin;
  rotMatix[10] = cos * relativeMatrix_[10];
}

void Transform::SetRelativeRotationY(const float _radian)
{
  float cos = cosf(_radian);
  float sin = sinf(_radian);

  relativeMatrix_[0] = cos;
  relativeMatrix_[2] = -sin;
  relativeMatrix_[8] = sin;
  relativeMatrix_[10] = cos;
}

void Transform::SetRelativeRotationZ(const float _radian)
{
  float cos = cosf(_radian);
  float sin = sinf(_radian);

  relativeMatrix_[0] = cos;
  relativeMatrix_[1] = -sin;
  relativeMatrix_[4] = sin;
  relativeMatrix_[5] = cos;
}