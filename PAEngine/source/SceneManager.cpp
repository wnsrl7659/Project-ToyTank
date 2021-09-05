#include "pch.h"
#include "SceneManager.h"
#include "SceneTest.h"
#include "ScenePlayerTest.h"
#include "SceneCastle.h"
#include "SceneMenu.h"
#include "SceneDigipen.h"

namespace PAEngine
{
	SceneManager::SceneManager(Window* _pAppWindow, EventProcessor* _pAProc)
	{
		pAppWindow = _pAppWindow;
		pAProc = _pAProc;
		scenes.max_load_factor(0.1f);
	}

	SceneManager::~SceneManager()
	{
		std::vector<std::string> names;

		for (auto sc : scenes)
		{
			names.push_back(const_cast<char*>(sc.first.c_str()));
		}
		for (auto nm : names)
		{
			RemoveScene(nm);
		}

		scenes.clear();
	}

	Scene* SceneManager::AddScene(std::string name, SceneType type)
	{
		Scene* sc = 0;
		
		switch(type)
		{
		case ST_test:
			sc = new SceneTest(pAppWindow, pAProc);
			break;
    case ST_playerTest:
      sc = new ScenePlayerTest(pAppWindow, pAProc);
      break;
    case ST_castle:
      sc = new SceneCastle(pAppWindow, pAProc);
      break;
    case ST_menu:
      sc = new SceneMenu(pAppWindow, pAProc);
      break;
	case ST_digipen:
		sc = new SceneDigipen(pAppWindow, pAProc);
		break;

		}
		if (sc == 0)
		{
			//error
			return 0;
		}
		scenes[name] = sc;
		
		return sc;
	}

	void SceneManager::RemoveScene(std::string name)
	{
		//check name is available
		delete scenes[name];
		scenes.erase(name);
	}

	void SceneManager::SetFirstScene(std::string name)
	{
		c_scene = name;
		n_scene = name;
	}

	void SceneManager::ChangeScene(std::string name)
	{
    if (scenes[name] == nullptr)
    {
      return;
    }
		n_scene = name;
	}

	void SceneManager::CheckScene()
	{
		if (n_scene[0] != c_scene[0])
		{
      if (n_scene[0] == "restart"[0])
      {
        scenes[c_scene]->Shutdown();
        n_scene = c_scene;
        scenes[c_scene]->Init();
      }
      else
      {
        scenes[c_scene]->Shutdown();
        c_scene = n_scene;
        scenes[c_scene]->Init();
      }
		}
	}
  void SceneManager::RestartScene()
  {
    n_scene = "restart";
  }

	void SceneManager::Init()
	{
		scenes[c_scene]->Init();
	}
	void SceneManager::AcquireInput(const double deltaTime)
	{
		scenes[c_scene]->AcquireInput(deltaTime);
	}
	void SceneManager::Update(const double deltaTime)
	{
		scenes[c_scene]->Update(deltaTime);
	}
	void SceneManager::PhysicsUpdate(const double deltaTime)
	{
		scenes[c_scene]->PhysicsUpdate(deltaTime);
	}
	void SceneManager::Render(const double farseer)
	{
		scenes[c_scene]->Render(farseer);
	}
	void SceneManager::Shutdown()
	{
		scenes[c_scene]->Shutdown();
	}
}