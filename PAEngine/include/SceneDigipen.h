/******************************************************************************
  * Author : Ryan Dugie
  * Class  : GAM 350
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once

#include "Scene.h"
#include "PlayerController.h"
#include "EnvironmentManager.h"

#include "../../PARender/inc/PARenderAPI/IRenderEngine.h"
#include "../../PARender/inc/PARenderAPI/IScene.h"

namespace PAEngine
{

	// this will be hendled by own manager. NEED TO FIX
	class SceneDigipen : public Scene
	{
	public:
		SceneDigipen(Window* _pAppWindow, EventProcessor* _pAProc) : Scene(_pAppWindow, _pAProc) {};
		void SceneInit();								          // initialize any valriables for this scene
		void SceneAcquireInput(TypeCommandActiveMap activeKeyMap, Gamepad* pGamepad = nullptr);	// user input for  this scene
		void SceneUpdate(const double deltaTime);		          // update any logic of this scene
		void ScenePhysicsUpdate(const double deltaTime);          // update any logic of this scene
		void SceneRender(const double farseer);			          // renders this scene
		void SceneShutdown();							          // shutdown any valriables for this scene
	private:
		int objectid;

		float timeToDisplay = 3.f;
	};
}