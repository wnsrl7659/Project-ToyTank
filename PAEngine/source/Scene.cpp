/**
 * @file Scene.cpp
 *
 * @brief An implementation of Scene.h
 *
 * @author Minsu Kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include "Scene.h"

#include "EventSystem.h"	// register and send events
#include "GameCommands.h"	// in-game control commands
#include "ObjectManager.h"
#include "PlayerBehavior.h"
#include "PlayerController.h"
#include "ObjectManager.h"
#include "ObjectFactory.h"
#include "Transform.h"
#include "Physics.h"
#include "playerWin.h"

#include "..//..//PARender/inc/PARenderAPI/IRenderEngine.h"



namespace PAEngine
{
  static PlayerWin pWin;
  //////////////////////////////////////////////////////////////////////////////
  // Scene /////////////////////////////////////////////////////////////////////

  Scene::Scene(Window* _pAppWindow, EventProcessor* _pAProc)
    : pAppWindow(_pAppWindow), pEventProc(_pAProc)
  {
    pLogger = Util::ServiceLocator::getLogger();
    pGameInput = Util::ServiceLocator::getGameInput();
  }

  Scene::~Scene()
  {
    // do not delete!
    pLogger = nullptr;
    pGameInput = nullptr;
    pEventProc = nullptr;
    pAppWindow = nullptr;
  }

  void Scene::Init()
  {
    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "Scene Init");
    ObjectManager::GetInstance()->Init();
    // This must be on the most bottom line
    this->SceneInit();
  }

  void Scene::AcquireInput(const double deltaTime)
  {
    pGameInput->AcquireInput(deltaTime);

    if (pGameInput->IsKeyboardActive())
    {
      // keyboard and mouse
      AcquireInputKeyCheck(pGameInput->GetKeyboardAndMouse()->activeKeyMap);
      this->SceneAcquireInput(pGameInput->GetKeyboardAndMouse()->activeKeyMap);
    }

    // gamepads
    if (pGameInput->IsGamepadActive())
    {
      std::vector<Gamepad*>::iterator it = pGameInput->GetGamePads().begin();
      while (it != pGameInput->GetGamePads().end())
      {
        AcquireInputKeyCheck((*it)->activeKeyMap, *it);
        this->SceneAcquireInput((*it)->activeKeyMap, *it);

        ++it;
      }
    }
  }

  void Scene::Update(const double deltaTime)
  {
    pEventProc->ProcessEvents();

    /////////////////////////////////////////////////////////////////////////////
    // FALLING //////////////////////////////////////////////////////////////////
    for (int i = 0; i < 4; i++)
    {
      int playerobjID = PlayerController::GetObjectIDFromPlayerID(i);
      if (playerobjID == -1)
      {
        break;
      }
      vec3 playerInitPos = PlayerController::GetObjectInitPosFromPlayerID(i);

      Physics* trans = ObjectManager::GetObjectByID(playerobjID)->GetComponent<Physics>();
      std::vector<float>& matrix = trans->GetMatrix();
      //will get ground y coord later
      if (matrix[13] < -10.0f)
      {
        if (PlayerWin::globalWin->GetWinner() == -1)
        {
          trans->Teleport(-matrix[12], -matrix[13], -matrix[14]);
          trans->Teleport(playerInitPos.x, playerInitPos.y, playerInitPos.z);
          trans->box->rigidbody->setLinearVelocity(btVector3(0, 0, 0));
          trans->box->rigidbody->setAngularVelocity(btVector3(0, 0, 0));
        }
        else
        {
          if (i == PlayerWin::globalWin->GetWinner())
          {
            trans->Teleport(-matrix[12], -matrix[13], -matrix[14]);
            trans->Teleport(playerInitPos.x, playerInitPos.y, playerInitPos.z);
            trans->box->rigidbody->setLinearVelocity(btVector3(0, 0, 0));
            trans->box->rigidbody->setAngularVelocity(btVector3(0, 0, 0));
          }
        }
      }
    }

    // This must be on the most bottom line
    this->SceneUpdate(deltaTime);
  }

  void Scene::PhysicsUpdate(const double deltaTime)
  {
    // This must be on the most bottom line
    this->ScenePhysicsUpdate(deltaTime);
  }

  void Scene::Render(const double farseer)
  {
    this->SceneRender(farseer);
  }

  void Scene::Shutdown()
  {
    ObjectManager::GetInstance()->Shutdown();
    // This must be on the TOP line, NOT BOTTOM!!
    this->SceneShutdown();
  }

  ///////////////////////////////////////////////////////////////////////////
  // Private Functions //////////////////////////////////////////////////////

	void Scene::AcquireInputKeyCheck(TypeCommandActiveMap activeKeyMap, Gamepad* pGamepad)
	{
    const int objectid = PlayerController::GetObjectIDFromPlayerID(pGamepad->GetPlayerID());
    if (objectid == -1)
    {
      return;
    }
    Physics* phys = ObjectManager::GetObjectByID(objectid)->GetComponent<Physics>();
    PlayerBehavior* behav = ObjectManager::GetObjectByID(objectid)->GetComponent<PlayerBehavior>();

		// act on user input
		for (auto x : activeKeyMap)
		{
			switch (x.first)
			{
      // aim movement
      case EventType::ET_INPUT_LOOK_UP_StillPressed:
        behav->AddCurrAimRadianY(0.01f);
        behav->thumbRightY = 1.f;
        break;
      case EventType::ET_INPUT_LOOK_UP_JustReleased:
        behav->thumbRightY = 0.f;
        break;
      case EventType::ET_INPUT_LOOK_DOWN_StillPressed:
        behav->AddCurrAimRadianY(-0.01f);
        behav->thumbRightY = -1.f;
        break;
      case EventType::ET_INPUT_LOOK_DOWN_JustReleased:
        behav->thumbRightY = 0.f;
        break;
      case EventType::ET_INPUT_LOOK_LEFT_StillPressed:
        behav->AddCurrAimRadianX(-0.03f);
        behav->thumbRightX = -1.f;
        break;
      case EventType::ET_INPUT_LOOK_LEFT_JustReleased:
        behav->thumbRightX = 0.f;
        break;
      case EventType::ET_INPUT_LOOK_RIGHT_StillPressed:
        behav->AddCurrAimRadianX(0.03f);
        behav->thumbRightX = 1.f;
        break;
      case EventType::ET_INPUT_LOOK_RIGHT_JustReleased:
        behav->thumbRightX = 0.f;
        break;
      // tank movement
			case EventType::ET_INPUT_FORWARD:
				EventSystem::SendEvent(ET_INPUT_FORWARD, (void*)pGamepad, sizeof(pGamepad));
				break;
			case EventType::ET_INPUT_BACKWARD:
				EventSystem::SendEvent(ET_INPUT_BACKWARD, (void*)pGamepad, sizeof(pGamepad));
				break;
			case EventType::ET_INPUT_LEFT:
				EventSystem::SendEvent(ET_INPUT_LEFT, (void*)pGamepad, sizeof(pGamepad));
				break;
			case EventType::ET_INPUT_RIGHT:
				EventSystem::SendEvent(ET_INPUT_RIGHT, (void*)pGamepad, sizeof(pGamepad));
				break;
			case EventType::ET_INPUT_DASH:
				EventSystem::SendEvent(ET_INPUT_DASH, (void*)pGamepad, sizeof(pGamepad));
				break;
      // Firing
			case EventType::ET_INPUT_FIRE:
				EventSystem::SendEvent(ET_INPUT_FIRE, (void*)pGamepad, sizeof(pGamepad));
				break; 
      case EventType::ET_INPUT_BEHAVIOR_RAPID_FIRE:
          EventSystem::SendEvent(ET_INPUT_BEHAVIOR_RAPID_FIRE, (void*)pGamepad, sizeof(pGamepad));
          break;
      case EventType::ET_INPUT_BEHAVIOR_PLATING:
        EventSystem::SendEvent(ET_INPUT_BEHAVIOR_PLATING, (void*)pGamepad, sizeof(pGamepad));
        break;
      // others
			case EventType::ET_INPUT_TEST:
				EventSystem::SendEvent(ET_INPUT_TEST, (void*)pGamepad, sizeof(pGamepad));
				break;
      case EventType::ET_INPUT_KILL:
        EventSystem::SendEvent(ET_INPUT_KILL, (void*)pGamepad, sizeof(pGamepad));
        break;
      case EventType::ET_INPUT_X:
        EventSystem::SendEvent(ET_GUIONOFF, (void*)nullptr, sizeof(nullptr));
        break;
			}
		}
	}
}