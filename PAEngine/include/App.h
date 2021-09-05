/**
 * @file App.h
 *
 * @brief All game-related components will be included in this class.
 *  This following funcions are what other programmers should use:
 *  Init, Shutdown, Update, Render, OnResize
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#ifndef APP_H
#define APP_H
#pragma once

#include <windows.h> // Microsoft
#include <string>

#include "Window.h" // PAE window class
#include "timer.h"
#include "EventProcessor.h"
#include "SceneManager.h"
#include "../../PARender/inc/PARenderAPI/IRenderEngine.h"
#include "..//..//PARender/inc/PARenderAPI/IScene.h"
typedef class PAGui;

Rendering::IEngine* GetEngineRender();
Rendering::IScene* GetSceneRender();

const std::vector<float> GetFPStimes();

namespace PAEngine
{

	class App
	{
	public:
		bool hasStarted;	// true if the app was started completely
		bool isDisabled;		// true if the game is paused 
    bool isPaused;
    
		Util::Timer* timer;	// high-precision timer

		App(HINSTANCE _hInstance);
		~App();

		bool Init();
		void Shutdown();

		// the main system loop (contains the window message loop)
		int Run();

		// to set the window attributes in Window.cpp
		inline HINSTANCE GetHInstance() { return m_appHInstance; }

		// user input
		void AcquireInput(const double deltaTime);

		// Need to implement by programmers
		void Update(const double deltaTime);			// update any logic of the game world
		void PhysicsUpdate(const double deltaTime);	// update any logic of the game physics
		void Render(const double farseer);			// renders the game world
		void OnResize();						// resize game graphics

    void ChangeScene(std::string name);
    void RestartScene();
	std::string CurrentScene() { return m_pSceneManager->CurrentScene(); }

    static App* globalApp;

    Window* m_pAppWindow;		// the application window
	private:
		// Utility Functions
		void CalculateFrameStatistics();	// computes frame statistics and displays using "SetWindowText"

		// application window
		HINSTANCE m_appHInstance;	// handle to an instance of the application
		SceneManager* m_pSceneManager;		
		EventProcessor* m_pEventProc;

		// to control frame rate
		int m_fps;							// frames per second
		double m_mspf;						// milliseconds per frame
		double m_dt;						// constant game update rate
		double m_gameLogicTargetFPS;		// target FPS for the game logic
		double m_gameLogicMaxSkipFrames;	// constant maximum of frames to skip in the update loop (important to not stall the system on slower computers)

		bool m_isContinueRunning;

    PAGui* mygui;
	};

}

#endif