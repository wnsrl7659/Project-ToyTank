/******************************************************************************
  * Author : Cody Morgan
  * Class  : GAM 300
  * Brief  : Manages player cameras

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#include "pch.h"
using std::vector;

#include "Physics.h"
#include "CameraManager.h"
#include "ChildTransformManager.h"
#include "ObjectManager.h" 
#include "../../PARender/inc/PARenderPCH.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include <DirectXMath.h>
using DirectX::XMMATRIX;
using DirectX::XMVECTOR;
typedef XMVECTOR vec4;
using DirectX::XMVector4Transform;
using DirectX::XMMatrixScalingFromVector;
using DirectX::XMMatrixMultiply;
using DirectX::XMVectorAdd;
using DirectX::XMVectorScale;
using DirectX::XMVector3Length;
using DirectX::XMVector4Normalize;

static CameraManager* cameraManager = nullptr;

void InitCameraManager(Rendering::IScene* sceneRender)
{
  if (!cameraManager)
    cameraManager = new CameraManager(sceneRender);

}

CameraManager* GetCameraManager()
{
  assert(cameraManager);
  return cameraManager;
}

void UpdateCameraManager(float dt)
{
  cameraManager->update(dt);
}


void ShutdownCameraManager()
{
  delete cameraManager;
  cameraManager = nullptr;
}

CameraManager::CameraManager(Rendering::IScene* sceneRender) :sceneRender_(sceneRender)
{
    width_ = Utility::GetWindowWidth();
    height_ = Utility::GetWindowHeight();
}

CameraComponent* CameraManager::addCamera(const int playerID, vec3 position)
{
  if (playerCount_ < 4)
  {
    if (cameraData[playerID].camera == nullptr)
      cameraData[playerID].camera = sceneRender_->AddCam();

    cameraData[playerID].camera->SetPosition(position.x, position.y, position.z);
    ++playerCount_;

    updateViewports();

    return &(cameraData[playerID]);
  }
}

void CameraManager::addMinimap()
{
  minimap.camera = sceneRender_->AddCam(0, 0, 4, 0, 1, 0, true);
  minimap.camera->SetPosition(0, 150, 0);
  minimap.camera->SetPitch(-1.5708f);
  minimap.camera->SetFOV(1.5708f);
  minimap.camera->SetFarPlane(200.f);
  minimap.camera->UseOrthoMatrix(true);

  updateViewports();
}

void CameraManager::addMegaMap()
{
  if (!megamap.camera)
  {
    megamap.camera = sceneRender_->AddCam(0, 0, 1, 0, 1, 0, true);
    megamap.camera->SetPosition(20, 1, 0); // magic number
    megamap.camera->SetPitch(-1.5708f);
    megamap.camera->SetFarPlane(200.f);
    megamap.camera->UseOrthoMatrix(true);
    megamap.viewport.w = width_;
    megamap.viewport.h = height_;
    megamap.viewport.x = 0;
    megamap.viewport.y = 0;
    #ifdef _DEBUG
    megamap.camera->SetAspectRatio(0.16f * width_, 0.16f * height_);
    #else
    megamap.camera->SetAspectRatio(0.1f * width_, 0.1f * height_);

    #endif
    megamap.camera->SetViewPort(megamap.viewport.w, megamap.viewport.h, megamap.viewport.x, megamap.viewport.y, 0.0f, 1.0f);
  }
}

void CameraManager::removeMegamap()
{
  sceneRender_->RemoveCam(megamap.camera);
  delete megamap.camera;
  megamap.camera = nullptr;
}

void CameraManager::removeCamera(unsigned playerNum)
{
  GameObject* object = ObjectManager::GetObjectByID(playerNum);
  //object->RemoveComponent<CameraComponent>();

  cameraData[playerNum].objectAttachmentID = -1;
  cameraData[playerNum].pAttachTrans = nullptr;

  sceneRender_->RemoveCam(cameraData[playerNum].camera);
  delete cameraData[playerNum].camera;
  cameraData[playerNum].camera = nullptr;

  --playerCount_;

  updateViewports();
}

void CameraManager::attachToObject(int objectID, int playerNum, vec3 positionOffset, vec3 rotationOffset)
{
  // a valid player
  if (playerNum < maxPlayer)
  {
    GameObject* object = ObjectManager::GetObjectByID(objectID);
    object->AddComponent(&(cameraData[playerNum]));

    cameraData[playerNum].objectAttachmentID = objectID;
    cameraData[playerNum].pAttachTrans = object->GetComponent<Transform>();
    cameraData[playerNum].vectorOffset = positionOffset;
    cameraData[playerNum].angleOffset = rotationOffset;
  }
}

void CameraManager::update(double dt)
{
  if (megamap.camera)
  {

  static float i = 0;
  i += 1;
  //megamap.camera->SetAspectRatio(width_-i, height_);
  //megamap.camera->SetPosition(i, 1, 0); // magic number

  }

  for (int i = 0; i < playerCount_; i++)
  {
    if (pCameraDataTempForViewportUpdate[i]->objectAttachmentID == -1)
      continue;

    // get object transform
    const float* trans = pCameraDataTempForViewportUpdate[i]->pAttachTrans->GetTransform();
    vec4 objPosition = { trans[12], trans[13], trans[14], trans[15] };
    
    updateShake(i);

    pCameraDataTempForViewportUpdate[i]->camera->SetPosition(
      objPosition.m128_f32[0] + pCameraDataTempForViewportUpdate[i]->vectorOffset.x + pCameraDataTempForViewportUpdate[i]->shakeOffset.x,
      objPosition.m128_f32[1] + pCameraDataTempForViewportUpdate[i]->vectorOffset.y + pCameraDataTempForViewportUpdate[i]->shakeOffset.y,
      objPosition.m128_f32[2] + pCameraDataTempForViewportUpdate[i]->vectorOffset.z + pCameraDataTempForViewportUpdate[i]->shakeOffset.z
    );

    pCameraDataTempForViewportUpdate[i]->camera->SetYaw(pCameraDataTempForViewportUpdate[i]->angleOffset.x);
    pCameraDataTempForViewportUpdate[i]->camera->SetPitch(pCameraDataTempForViewportUpdate[i]->angleOffset.y);
  }
}

void CameraManager::shutdown()
{

}

void CameraManager::setCameraOffsetVector(int playerNum, vec3 cameraOffset)
{
  cameraData[playerNum].vectorOffset = cameraOffset;

}

void CameraManager::setCameraOffsetRotation(int playerNum, vec3 cameraOffset)
{
  cameraData[playerNum].angleOffset = cameraOffset;
}

bool CameraManager::isActivated(const int playerID)
{
  if (cameraData[playerID].camera == nullptr)
    return false;

  return true;
}

void CameraManager::updateViewports()
{
  int i = 0;
  for (auto& eachCameraData : cameraData)
  {
    if (eachCameraData.camera != nullptr)
    {
      pCameraDataTempForViewportUpdate[i] = &eachCameraData;
      ++i;
    }
  }

  switch (playerCount_)
  {
    // R B Y G

    // [0][1]
    //  [M]
    // [2][3]
  case 4:
    // player0
    pCameraDataTempForViewportUpdate[0]->viewport.x = 0;
    pCameraDataTempForViewportUpdate[0]->viewport.y = 0;
    pCameraDataTempForViewportUpdate[0]->viewport.w = width_ / 2;
    pCameraDataTempForViewportUpdate[0]->viewport.h = height_ / 2;

    // player1
    pCameraDataTempForViewportUpdate[1]->viewport.x = width_ / 2;
    pCameraDataTempForViewportUpdate[1]->viewport.y = 0;
    pCameraDataTempForViewportUpdate[1]->viewport.w = width_ / 2;
    pCameraDataTempForViewportUpdate[1]->viewport.h = height_ / 2;

    // player2
    pCameraDataTempForViewportUpdate[2]->viewport.x = 0;
    pCameraDataTempForViewportUpdate[2]->viewport.y = height_ / 2;
    pCameraDataTempForViewportUpdate[2]->viewport.w = width_ / 2;
    pCameraDataTempForViewportUpdate[2]->viewport.h = height_ / 2;

    // player3
    pCameraDataTempForViewportUpdate[3]->viewport.x = width_ / 2;
    pCameraDataTempForViewportUpdate[3]->viewport.y = height_ / 2;
    pCameraDataTempForViewportUpdate[3]->viewport.w = width_ / 2;
    pCameraDataTempForViewportUpdate[3]->viewport.h = height_ / 2;

    pCameraDataTempForViewportUpdate[0]->camera->SetAspectRatio(width_ / 2, height_);
    pCameraDataTempForViewportUpdate[1]->camera->SetAspectRatio(width_ / 2, height_);
    pCameraDataTempForViewportUpdate[2]->camera->SetAspectRatio(width_ / 2, height_);
    pCameraDataTempForViewportUpdate[3]->camera->SetAspectRatio(width_ / 2, height_);

    //miniMap
    minimap.viewport.x = (3 * width_) / 8.0f;
    minimap.viewport.y = (3 * height_) / 8.0f;
    minimap.viewport.w = width_ / 4;
    minimap.viewport.h = height_ / 4;
    break;

    // [0][1]
    // [2][M]
  case 3:
    // player0
    pCameraDataTempForViewportUpdate[0]->viewport.x = width_ / 4;
    pCameraDataTempForViewportUpdate[0]->viewport.y = 0;
    pCameraDataTempForViewportUpdate[0]->viewport.w = width_ / 2;
    pCameraDataTempForViewportUpdate[0]->viewport.h = height_ / 2;

    // player1
    pCameraDataTempForViewportUpdate[1]->viewport.x = 0;
    pCameraDataTempForViewportUpdate[1]->viewport.y = height_ / 2;
    pCameraDataTempForViewportUpdate[1]->viewport.w = width_ / 2;
    pCameraDataTempForViewportUpdate[1]->viewport.h = height_ / 2;

    // player2
    pCameraDataTempForViewportUpdate[2]->viewport.x = width_ / 2;
    pCameraDataTempForViewportUpdate[2]->viewport.y = height_ / 2;
    pCameraDataTempForViewportUpdate[2]->viewport.w = width_ / 2;
    pCameraDataTempForViewportUpdate[2]->viewport.h = height_ / 2;

    pCameraDataTempForViewportUpdate[0]->camera->SetAspectRatio(width_ / 2, height_);
    pCameraDataTempForViewportUpdate[1]->camera->SetAspectRatio(width_ / 2, height_);
    pCameraDataTempForViewportUpdate[2]->camera->SetAspectRatio(width_ / 2, height_);

    //miniMap
    minimap.viewport.x = width_ / 2;
    minimap.viewport.y = height_ / 2;
    minimap.viewport.w = width_ / 2;
    minimap.viewport.h = height_ / 2;
    break;

    // [0]
    // [M]
    // [1]
  case 2:
    // player0
    pCameraDataTempForViewportUpdate[0]->viewport.x = 0;
    pCameraDataTempForViewportUpdate[0]->viewport.y = 0;
    pCameraDataTempForViewportUpdate[0]->viewport.w = width_;
    pCameraDataTempForViewportUpdate[0]->viewport.h = height_ / 2;

    // player1
    pCameraDataTempForViewportUpdate[1]->viewport.x = 0;
    pCameraDataTempForViewportUpdate[1]->viewport.y = height_ / 2;
    pCameraDataTempForViewportUpdate[1]->viewport.w = width_;
    pCameraDataTempForViewportUpdate[1]->viewport.h = height_ / 2;

    pCameraDataTempForViewportUpdate[0]->camera->SetAspectRatio(width_, height_);
    pCameraDataTempForViewportUpdate[1]->camera->SetAspectRatio(width_, height_);

    //miniMap
    minimap.viewport.x = (3 * width_) / 8.0f;
    minimap.viewport.y = (3 * height_) / 8.0f;
    minimap.viewport.w = width_ / 4;
    minimap.viewport.h = height_ / 4;
    break;

    // [0][M]
  case 1:
    // player0
    pCameraDataTempForViewportUpdate[0]->viewport.x = 0;
    pCameraDataTempForViewportUpdate[0]->viewport.y = 0;
    pCameraDataTempForViewportUpdate[0]->viewport.w = width_;
    pCameraDataTempForViewportUpdate[0]->viewport.h = height_;

    pCameraDataTempForViewportUpdate[0]->camera->SetAspectRatio(width_ / 2.f, height_);

    //miniMap
    minimap.viewport.x = width_ / 2;
    minimap.viewport.y = 0;
    minimap.viewport.w = width_ / 2;
    minimap.viewport.h = height_ / 2;

    break;

  default:
    break;
  }

  // reset the cameras to correct viewport
  for (int i = 0; i < playerCount_; i++)
  {
    if (pCameraDataTempForViewportUpdate[i]->camera != nullptr)
      pCameraDataTempForViewportUpdate[i]->camera->SetViewPort(
        pCameraDataTempForViewportUpdate[i]->viewport.w, pCameraDataTempForViewportUpdate[i]->viewport.h,
        pCameraDataTempForViewportUpdate[i]->viewport.x, pCameraDataTempForViewportUpdate[i]->viewport.y,
        0.0f, 1.0f
      );
  }

  if (minimap.camera)
  {
    minimap.camera->SetAspectRatio(width_ / 3, height_ / 3);
    minimap.camera->SetViewPort(minimap.viewport.w, minimap.viewport.h, minimap.viewport.x, minimap.viewport.y, 0.0f, 1.0f);
  }
}

void CameraManager::updateShake(int i) {
  CameraComponent& cam = cameraData[i];
  
  if (cam.shakePoints > cam.shakePointsHit) {
    if (cam.currentShakeDuration % (cam.shakeDuration / cam.shakePoints) == 0) {
      if (cam.shakePointsHit == cam.shakePoints - 1) {
        cam.shakeVelocity = (cam.shakeOffset);
        cam.shakeVelocity = cam.shakeVelocity * -1;
      }
      else {
        // get the next velocity for the camera 
        float x_offset = RNG::GetRNG().RandFloat(-cam.shakeRadius, cam.shakeRadius);
        float y_offset = RNG::GetRNG().RandFloat(-cam.shakeRadius, cam.shakeRadius);
        vec3 newShakePoint(cam.vectorOffset.x, cam.vectorOffset.y + y_offset, cam.vectorOffset.z + x_offset);

        vec3 currentPos = cam.shakeOffset + cam.vectorOffset;
        cam.shakeVelocity = newShakePoint - currentPos;

      }
      cam.shakePointsHit++;
      cam.shakeVelocity* (1.f / float(cam.shakeDuration / cam.shakePoints));
    } 
    if (cam.currentShakeDuration < cam.shakeDuration ) {
      // calculate next offset while the camera is shaking
      cam.shakeOffset += cam.shakeVelocity;
      cam.currentShakeDuration++;
    }
  }
  else {
    cam.shakeOffset = { 0,0,0 };
  }
}
