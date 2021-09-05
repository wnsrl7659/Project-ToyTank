/**
 * @file SceneTest.h
 *
 * @brief It's a test scene, a sandbox.
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

#include "Scene.h"
#include "PlayerController.h"

namespace PAEngine
{
	// this will be hendled by own manager. NEED TO FIX
	class SceneTest : public Scene
	{
	public:
		SceneTest(Window* _pAppWindow, EventProcessor* _pAProc) : Scene(_pAppWindow, _pAProc) {};
		void SceneInit();								          // initialize any valriables for this scene
		void SceneAcquireInput(TypeCommandActiveMap activeKeyMap, Gamepad* pGamepad = nullptr);	// user input for  this scene
		void SceneUpdate(const double deltaTime);		          // update any logic of this scene
		void ScenePhysicsUpdate(const double deltaTime);          // update any logic of this scene
		void SceneRender(const double farseer);			          // renders this scene
		void SceneShutdown();							          // shutdown any valriables for this scene
	private:
    int objectid;
		void ThisIsAPrivateFunctionOfSceneTest();
    PlayerController playerController_;
	};

	void KeyChanging(void* data);
}