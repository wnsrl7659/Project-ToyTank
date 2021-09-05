/******************************************************************************
  filename: ChildTransform.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once
#include "Component.h"
#include <array>

class Transform : public Component {
public:
  Transform(int managerIdx, ComponentManager* manager);
  ~Transform();
  Transform* Create(int objectID, float scaleX, float scaleY, float scaleZ,
    float relativeX, float relativeY, float relativeZ);
  void Destroy() override;

  const float* GetTransform() const;

  // getter
  float GetPositionX() const;
  float GetPositionY() const;
  float GetPositionZ() const;
  void  GetPosition(float* position) const;
  float GetRelativePositionX() const { return relativeMatrix_[3]; }
  float GetRelativePositionY() const { return relativeMatrix_[7]; }
  float GetRelativePositionZ() const { return relativeMatrix_[11]; }
  float GetScaleX() const { return transformMatrix_[0]; }
  float GetScaleY() const { return transformMatrix_[5]; }
  float GetScaleZ() const { return transformMatrix_[10]; }

  // setter
  //inline void SetPositionX(const float _x) { transformMatrix_[3] = _x; }
  //inline void SetPositionY(const float _y) { transformMatrix_[7] = _y; }
  //inline void SetPositionZ(const float _z) { transformMatrix_[11] = _z; }
  inline void SetRelativePosition(const float _x, const float _y, const float _z);
  inline void SetRelativePositionX(const float _x) { relativeMatrix_[3] = _x; }
  inline void SetRelativePositionY(const float _y) { relativeMatrix_[7] = _y; }
  inline void SetRelativePositionZ(const float _z) { relativeMatrix_[11] = _z; }
  void SetRelativeRotationX(const float _radian);
  void SetRelativeRotationY(const float _radian);
  void SetRelativeRotationZ(const float _radian);
  inline void SetPositionY(float y) { transformMatrix_[13] = y; }

private:
  std::array<float, 16> relativeMatrix_;  // local position
  std::array<float, 16> transformMatrix_; // world position

  friend class TransformManager;
};
