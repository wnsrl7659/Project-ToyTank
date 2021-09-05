/******************************************************************************
  file: PlayerController.h
  author: j.mcdonell

  The player controller is used to handle input events coming from game pads
  and pass them on to the corresponding player for further processng
******************************************************************************/
#pragma once
#include <array>
#include <queue>
#include <Player.h>
#include "CameraManager.h"

#define MAX_PLAYERS 4
class PAEngine::Gamepad;
class PlayerController {
public:
  PlayerController();
  ~PlayerController();
  void Init();
  void Update(float dt);
  void Shutdown();

  static void PairPlayerToGamepad(unsigned playerID, PAEngine::Gamepad* gamepad);
  void PairPlayersToGamepads();
  void PairRemainingPlayersToAI();
  void PairPlayerToAI(unsigned playerID);

  static int GetObjectIDFromPlayerID(const unsigned playerID);
  static int GetObjectIDFromGamePad(PAEngine::Gamepad* gamepad);
  static vec3 GetObjectInitPosFromPlayerID(const unsigned playerID);
  static Player* GetPlayerData(const unsigned playerID) { return &players_[playerID]; }

private:
  static std::array<Player, MAX_PLAYERS> players_;

};