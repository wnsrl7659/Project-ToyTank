/**
 * @file Scene.h
 *
 * @brief This is the base class of all scenes:
 *   - public functions
     will be used by SceneManager
 *     will be called for all derived secnes
 *   - protected functions
     will be used by programming artiests
 *     will only be called for its derived scene
 *   - private functions must not be used by anyone
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

#include "Window.h" // PAE window class
#include "ServiceLocator.h"
#include "EventProcessor.h"

namespace PAEngine
{
	//////////////////////////////////////////////////////////////////////////////
	// Scene /////////////////////////////////////////////////////////////////////

	// This is the base class of all scenes
	class Scene
	{
	// SceneManager will only use this functions
	public:
		Scene(Window* _pAppWindow, EventProcessor* _pAProc);
		~Scene();

		// the entire scenes will use this functions
		// you must write implementation which is required for all scenes in common
		void Init();								// initialize any valriables for this scene
		void AcquireInput(const double deltaTime);						// user input for  this scene
		void Update(const double deltaTime);		// update any logic of this scene
		void PhysicsUpdate(const double deltaTime);	// update any logic of this scene
		void Render(const double farseer);			// renders this scene
		void Shutdown();							// shutdown any valriables for this scene

	// Our programming artiests will only use this functions
	protected:
		// requirement
		Window* pAppWindow;
		EventProcessor* pEventProc;
    

		// objects from outside (service locators)
		Util::Logger* pLogger;
    GameInput* pGameInput;

		// each scene will use this own functions
		virtual void SceneInit() = 0;
		virtual void SceneAcquireInput(TypeCommandActiveMap activeKeyMap, Gamepad* pGamepad = nullptr) = 0;
		virtual void SceneUpdate(const double deltaTime) = 0;
		virtual void ScenePhysicsUpdate(const double deltaTime) = 0;
		virtual void SceneRender(const double farseer) = 0;
		virtual void SceneShutdown() = 0;

	// do not use this functions
	private:
		// use the parameterized constructor becuase pAppWindow must be initialized
		Scene() {};
		void AcquireInputKeyCheck(TypeCommandActiveMap activeKeyMap, Gamepad* pGamepad = nullptr);
	};
}