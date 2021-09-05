#pragma once
#include "Component.h"
#include "ComponentManager.h"
#include "../../PARender/inc/PARenderAPI/IScene.h"
#include "ChildTransform.h"

#include <stack>

class CameraComponent : public Component{
public:
  struct viewport
  {
    float x;
    float y;

    float w;
    float h;
  };

  CameraComponent();
  ~CameraComponent();

  void StartShake(int duration, int shakePoints, float radius);

  CameraComponent* Create();
  void Destroy();

private:
  ICamera* camera = nullptr;          // pointer to actual cameras
  viewport viewport;                  // calculated viewports depending on player count

  int objectAttachmentID = -1;        // object camera is attached to (if any)
  Transform* pAttachTrans = nullptr;
  vec3 vectorOffset;                  // the vector offset that the camera should maintain
  vec3 shakeOffset;                   // offset from the base offset
  vec3 angleOffset;                   
  vec3 shakeVelocity;                 // change the shakeOffset by this amount every frame

  int shakeDuration; // total frame duration of camera shake until it stops
  int currentShakeDuration; // current frame duaration of the shake
  int shakePoints; // number of points the shake will go to
  int shakePointsHit;
  float shakeRadius; // range of the shake


  friend class CameraManager;
};

