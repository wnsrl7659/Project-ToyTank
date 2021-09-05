/******************************************************************************
  file: ObjectFactory.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#include "InputDevices.h"
#include "ModelComponent.h"
#include "CameraManager.h"

namespace ObjectFactory {
  int CreateGround(vec3 position = vec3(0,-4,0), vec3 size = vec3(200,1,200), int index = -1);
  int CreatePlayer(PAEngine::Gamepad* gamepad, float xPos, float yPos, float zPos);
  int CreateAI(float xPos, float yPos, float zPos, bool moveVertical);
  int CreateFlag();
  int CreatePowerUp();
  int CreateGauge(const int ID, const int targetObjectID);
  int CreateBox(float scaleX, float scaleY, float scaleZ, float mass, float offsetX, float offsetY, float offsetZ, ModelType type = MT_CUBE_METALGRID);
  int CreateChildBox(float scaleX, float scaleY, float scaleZ, float offsetX, float offsetY, float offsetZ);
  int CreateBullet(int shooter, float posX, float posY, float posZ, float dirX, float dirY, float dirZ, bool explode = false, bool damagePlayer = true, float bulletSize = 0.5f, float bulletSpeed = 25.f);
  int CreateAstar();
  int CreateFuelDepot(float offsetX, float offsetY, float offsetZ);
  int CreateTutorial();
}

