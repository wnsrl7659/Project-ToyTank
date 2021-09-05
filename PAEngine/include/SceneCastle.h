/******************************************************************************
  * Author : Junki Kim
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once

#include "Scene.h"
#include "PlayerController.h"
#include "EnvironmentManager.h"


namespace PAEngine
{

	// this will be hendled by own manager. NEED TO FIX
	class SceneCastle : public Scene
	{
	public:
    SceneCastle(Window* _pAppWindow, EventProcessor* _pAProc) : Scene(_pAppWindow, _pAProc) {};
		void SceneInit();								          // initialize any valriables for this scene
		void SceneAcquireInput(TypeCommandActiveMap activeKeyMap, Gamepad* pGamepad = nullptr);	// user input for  this scene
		void SceneUpdate(const double deltaTime);		          // update any logic of this scene
		void ScenePhysicsUpdate(const double deltaTime);          // update any logic of this scene
		void SceneRender(const double farseer);			          // renders this scene
		void SceneShutdown();							          // shutdown any valriables for this scene
	private:
    int objectid;
    PlayerController playerController_;
    void GenerateCastle(EnvironmentManager* enviro);
    void GenerateRandomWalls(EnvironmentManager* enviro, int quantity);
		void GenerateRandomBuildings(EnvironmentManager* enviro, int quantity);
    void GenerateBoundry(EnvironmentManager* enviro, float height = 10);

	};
}