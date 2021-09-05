/******************************************************************************
  * Author : Cody Morgan
  * Class  : GAM 300
  * Brief  : Manages player cameras

  (c) DigiPen Institute of Technology 2019
******************************************************************************/
#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H
#pragma once
#include "../../PARender/inc/PARenderAPI/IRenderEngine.h"
#include "CameraComponent.h"
#include "ComponentManager.h"

constexpr int maxPlayer = 4;

class CameraManager{
public:

  CameraManager(Rendering::IScene* sceneRender);

  CameraComponent* addCamera(const int playerID, vec3 location);
  void addMinimap();
  void addMegaMap();
  void removeMegamap();
  void removeCamera(unsigned playerNum);
  void attachToObject(int objectID, int playerNum, vec3 positionOffset, vec3 rotationOffset = vec3(0.f));
  
  void update(double dt);
  void shutdown();

  // Getter
  inline const Transform* const getAttachmentTransform(const int playerNum) { return cameraData[playerNum].pAttachTrans; } const
  inline const vec3 getCameraOffsetVecter(const int playerNum) const { return cameraData[playerNum].vectorOffset; }
  inline const vec3 getCameraOffsetAngle(const int playerNum) const { return cameraData[playerNum].angleOffset; }

  // Setter
  void setCameraOffsetVector(int playerNum, vec3 cameraOffset);
  void setCameraOffsetRotation(int playerNum, vec3 cameraOffset);

  bool isActivated(const int playerID);

private:

  void updateViewports();
  void updateShake(int i);
                      
  CameraComponent cameraData[maxPlayer]; // one for each player max : 4
  // for ViewportUpdate and CameraUpdate (to solve the order problem)
  CameraComponent* pCameraDataTempForViewportUpdate[maxPlayer]; // ask Minsu for the detailed purpose
  CameraComponent minimap;               // minimap camera
  CameraComponent megamap;               // minimap camera
  unsigned playerCount_ = 0;        // how many players are there max = 4

  Rendering::IScene* sceneRender_;
  unsigned height_;
  unsigned width_;

  static CameraManager* cameraManager;
};

// create a camera management system for this scene
void InitCameraManager(Rendering::IScene* sceneRender);
CameraManager* GetCameraManager();
void UpdateCameraManager(float dt);
void ShutdownCameraManager();

#endif