#include "pch.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent() : Component(-1, nullptr), shakePoints(0), shakePointsHit(0) {
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::StartShake(int duration, int shakePoints, float radius) {
  shakeRadius = radius;
  this->shakePoints = shakePoints;
  shakePointsHit = 0;
  // set shake durations
  shakeDuration = duration;
  currentShakeDuration = 0;
  // everything else is taken care of in CameraManager::updateShake
}

CameraComponent* CameraComponent::Create()
{
  return nullptr;
}

void CameraComponent::Destroy()
{
}
