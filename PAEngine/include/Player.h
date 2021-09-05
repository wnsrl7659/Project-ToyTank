/******************************************************************************
  file: Player.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#include "InputDevices.h"
#include "CameraManager.h"

class ChangeT;
class Behavior;
enum PlayerInputType {
  PIT_INVALID = -1,
  PIT_GAMEPAD_0,
  PIT_GAMEPAD_1,
  PIT_GAMEPAD_2,
  PIT_GAMEPAD_3,
  PIT_AI_0
};

class Player {
public:
  Player();
  ~Player();

  void Init(const int ID);
  void Shutdown();

  void RecreateObject(float _posX, float _posY, float _posZ);

  PlayerInputType GetInputType();
  void RecieveInput();
private:
  PlayerInputType inputType_;
  int objectID_;
  vec3 InitPos_;
  vec3 color;

  PAEngine::Gamepad* gamepad_;
  Behavior* behav_;

  friend class PlayerController;
};

