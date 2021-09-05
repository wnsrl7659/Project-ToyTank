/******************************************************************************
  * Author : Junki Kim
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once

#include <string>
#include <unordered_map>

#include "Scene.h"
#include "Window.h"
#include "EventProcessor.h"

typedef enum SceneType
{
  ST_default = -1,
	ST_test,
  ST_playerTest,
  ST_castle,
  ST_menu,
  ST_digipen,
	ST_count,
}SceneType;

namespace PAEngine
{
	class SceneManager
	{
	public:
		SceneManager(Window* _pAppWindow, EventProcessor* _pAProc);
		~SceneManager();
		Scene* AddScene(std::string name, SceneType type);
		void RemoveScene(std::string name);
		void SetFirstScene(std::string name);		
		void ChangeScene(std::string name);
    void CheckScene();
    void RestartScene();

		void Init();								
		void AcquireInput(const double deltaTime);
		void Update(const double deltaTime);		
		void PhysicsUpdate(const double deltaTime);	
		void Render(const double farseer);			
		void Shutdown();	

		std::string CurrentScene() { return c_scene; }

	private:
		std::unordered_map<std::string, Scene*> scenes;
		Window* pAppWindow;
		EventProcessor* pAProc;
		std::string c_scene;
		std::string n_scene;
	};
}