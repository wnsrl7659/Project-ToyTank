#include "pch.h"
#include "SceneCastle.h"
#include "EventSystem.h"
#include "ObjectManager.h"
#include "Transform.h"
#include "ChildTransform.h"
#include "ModelComponent.h"
#include "ObjectFactory.h"
#include "App.h"
#include "PAGuiWidgets.h"
#include "PAGui.h"
#include "PlayerWin.h"
#include "ModelManager.h"


namespace PAEngine
{

  void SceneCastle::SceneInit()
  {
    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "castle Init\n");

    /////////////////////
    // Keyboard and Mouse
    pGameInput->SetGameInputKBM(EventType::ET_INPUT_KEY_CHANGING,
      new GameCommand(VK_RSHIFT, KeyState::JustPressed),
      nullptr);

    /////////
    // XInput
    playerController_.Init();

    // 4 TANKS (AI)
    const float posInitAI = 100.f; // 175
    ObjectFactory::CreateAI(0.f, 0.f, posInitAI, true);
    ObjectFactory::CreateAI(-posInitAI, 0.f, 0.f, false);
    ObjectFactory::CreateAI(0.f, 0.f, -posInitAI, true);
    ObjectFactory::CreateAI(posInitAI + 37, 0.f, 0.f, false);

    // create ground
    EnvironmentManager* enviro = EnvironmentManager::getEnvironment();
    enviro->generateGround(200,19);

    // create flag
    int flag = ObjectFactory::CreateFlag();
    int astar = ObjectFactory::CreateAstar();
    int refuel = ObjectFactory::CreateFuelDepot(0,-2.97,0);
#ifndef _DEBUG
    GenerateCastle(enviro);
    GenerateRandomBuildings(enviro, 1);
#endif
    GenerateRandomBuildings(enviro, 1);
    GenerateRandomWalls(enviro, 5);
    GenerateBoundry(enviro, 10);

    EventSystem::SendEvent(ET_SOUND_L_START_GAMEMUSIC, nullptr, 0);
  }

  void SceneCastle::SceneAcquireInput(TypeCommandActiveMap activeKeyMap, Gamepad* pGamepad)
  {
    // act on user input
    for (auto x : activeKeyMap)
    {
      Physics* phys = ObjectManager::GetObjectByID(objectid)->GetComponent<Physics>();
      Transform* trans = ObjectManager::GetObjectByID(objectid)->GetComponent<Transform>();
      switch (x.first)
      {
      case EventType::ET_INPUT_START:
          break;
      case EventType::ET_INPUT_QUIT:
          if (!confirmGUI)
          {
              pausetab = 0;
              PAEngine::App::globalApp->isPaused = !PAEngine::App::globalApp->isPaused;
              EventSystem::SendEvent(ET_UIONOFF, (void*)nullptr, sizeof(nullptr));
          }
          break;

      case EventType::ET_INPUT_MU:
          if (!confirmGUI)
          {
              if (PAEngine::App::globalApp->isPaused)
              {
                EventSystem::SendEvent(ET_SOUND_OS_BUTTONNAVIGATION, nullptr, 0);

                  if (pausetab > 0)
                  {
                      pausetab--;
                  }
              }
          }
          break;
      case EventType::ET_INPUT_MD:
          if (!confirmGUI)
          {
              if (PAEngine::App::globalApp->isPaused)
              {
                EventSystem::SendEvent(ET_SOUND_OS_BUTTONNAVIGATION, nullptr, 0);

                  if (pausetab < 2)
                  {
                      pausetab++;
                  }
              }
          }
        break;
      case EventType::ET_INPUT_ML:

          if (!confirmGUI)
          {
              if (!PAEngine::App::globalApp->isPaused && winGUI)
              {
                EventSystem::SendEvent(ET_SOUND_OS_BUTTONNAVIGATION, nullptr, 0);

                  if (wintab > 0)
                  {
                      wintab--;
                  }
              }
          }
          else if (confirmGUI)
          {
              if (confirmtab > 0)
              {
                  confirmtab--;
              }
          }
          break;
      case EventType::ET_INPUT_MR:
          if (!confirmGUI)
          {
              if (!PAEngine::App::globalApp->isPaused && winGUI)
              {
                EventSystem::SendEvent(ET_SOUND_OS_BUTTONNAVIGATION, nullptr, 0);

                  if (wintab < 1)
                  {
                      wintab++;
                  }
              }
          }
          else if (confirmGUI)
          {
              if (confirmtab < 1)
              {
                  confirmtab++;
              }
          }
          break;
      case EventType::ET_INPUT_A:
          if (!confirmGUI)
          {
              if (PAEngine::App::globalApp->isPaused)
              {
                EventSystem::SendEvent(ET_SOUND_OS_BUTTONSELECT, nullptr, 0);

                  if (pausetab == 0)
                  {
                      EventSystem::SendEvent(ET_PAUSEONOFF, (void*)nullptr, sizeof(nullptr));
                      EventSystem::SendEvent(ET_UIONOFF, (void*)nullptr, sizeof(nullptr));
                  }
                  else if (pausetab == 1)
                  {
                      EventSystem::SendEvent(ET_PAUSEONOFF, (void*)nullptr, sizeof(nullptr));
                      PAEngine::App::globalApp->RestartScene();
                      EventSystem::SendEvent(ET_UIONOFF, (void*)nullptr, sizeof(nullptr));
                  }
                  else if (pausetab == 2)
                  {
                      EventSystem::SendEvent(ET_CONFIRMONOFF, (void*)nullptr, sizeof(nullptr));
                  }
              }
              else
              {
                  if (winGUI)
                  {
                      if (wintab == 0)
                      {
                          if (GUIwinner != -1)
                          {
                              GetCameraManager()->removeMegamap();
                              PAEngine::App::globalApp->RestartScene();
                              PlayerWin::globalWin->SetWinner(-1);
                              EventSystem::SendEvent(ET_WINONOFF, (void*)nullptr, sizeof(nullptr));
                              EventSystem::SendEvent(ET_UIONOFF, (void*)nullptr, sizeof(nullptr));
                          }
                      }
                      else if (wintab == 1)
                      {
                          EventSystem::SendEvent(ET_CONFIRMONOFF, (void*)nullptr, sizeof(nullptr));
                      }
                  }
              }
          }
          else
          {
              if (confirmtab == 0)
              {
                  Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "ET_INPUT_QUIT");
                  PostMessage(pAppWindow->GetMainWindowHandle(), WM_CLOSE, 0, 0);
              }
              else if (confirmtab == 1)
              {
                  EventSystem::SendEvent(ET_CONFIRMONOFF, (void*)nullptr, sizeof(nullptr));
              }
          }
        break;
      case EventType::ET_INPUT_KEY_CHANGING:
        break;
      }
    }
  }

  void SceneCastle::SceneUpdate(const double deltaTime)
  {

    //print matrix of current object
    /*
    const float* matrix = ComponentManager::globalCM->ptransform->getTransform(objectid);
    char temp[1000];
    sprintf_s(temp, "\n %f, %f, %f, %f \n %f %f %f %f \n %f %f %f %f \n %f %f %f %f \n",
      matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5], matrix[6], matrix[7],
      matrix[8], matrix[9], matrix[10], matrix[11], matrix[12], matrix[13], matrix[14], matrix[15]);
    Util::ServiceLocator::getFileLogger()->print(Util::SeverityType::info, temp);
    */
  }

  void SceneCastle::ScenePhysicsUpdate(const double deltaTime)
  {

  }

  void SceneCastle::SceneRender(const double farseer) {
  }

  void SceneCastle::SceneShutdown()
  {
    playerController_.Shutdown();
    ObjectManager::ClearObjects();
    EventSystem::SendEvent(ET_SOUND_L_END_GAMEMUSIC, nullptr, 0);

  }

  // generate our castle
  void SceneCastle::GenerateCastle(EnvironmentManager* enviro)
  {
    //destructable wall
    enviro->generateWall(5, 3, vec3(-25, -2, -25), 0, false);
    enviro->generateWall(5, 4, vec3(-30, -2, -30), 0, false);
    //inside wall
    enviro->generateWall(5, 3, vec3(-15, -2, -25),0, false);
    enviro->generateWall(5, 3, vec3(-5, -2, -25),0, false);
    enviro->generateWall(5, 4, vec3(5, -2, -25),0, false);
    enviro->generateWall(5, 3, vec3(15, -2, -25),0, false);
    //                      3 
    enviro->generateWall(5, 3, vec3(-25, -2, 25),0, false);
    enviro->generateWall(5, 3, vec3(-15, -2, 25),0, false);
    enviro->generateWall(5, 3, vec3(-5, -2, 25),0, false);
    enviro->generateWall(5, 3, vec3(5, -2, 25),0, false);
    enviro->generateWall(5, 3, vec3(15, -2, 25),0, false);
    //                      3 
    enviro->generateWall(5, 3, vec3(-35, -2, -25), 90, false);
    enviro->generateWall(5, 4, vec3(-35, -2, -15), 90, false);
    enviro->generateWall(5, 3, vec3(-35, -2, -5), 90, false);
    enviro->generateWall(5, 3, vec3(-35, -2, 5), 90, false);
    enviro->generateWall(5, 3, vec3(-35, -2, 15), 90, false);
    //                      3 
    enviro->generateWall(5, 3, vec3(15, -2, -25), 90, false);
    enviro->generateWall(5, 4, vec3(15, -2, -15), 90, false);
    enviro->generateWall(5, 3, vec3(15, -2, -5), 90, false);
    enviro->generateWall(5, 3, vec3(15, -2, 5), 90, false);
    enviro->generateWall(5, 3, vec3(15, -2, 15), 90, false);
    //outside               3
    enviro->generateWall(5, 3, vec3(-50, -2, -50),0, false);
    enviro->generateWall(5, 3, vec3(-40, -2, -50),0, false);
    enviro->generateWall(5, 3, vec3(-30, -2, -50),0, false);
    enviro->generateWall(5, 3, vec3(-20, -2, -50),0, false);
    enviro->generateWall(5, 4, vec3(-10, -2, -50),0, false);
    enviro->generateWall(5, 4, vec3(0, -2, -50),0, false);
    enviro->generateWall(5, 4, vec3(10, -2, -50),0, false);
    enviro->generateWall(5, 3, vec3(20, -2, -50),0, false);
    enviro->generateWall(5, 3, vec3(30, -2, -50),0, false);
    enviro->generateWall(5, 3, vec3(40, -2, -50),0, false);
    //                      3 
    enviro->generateWall(5, 3, vec3(-50, -2, 50),0, false);
    enviro->generateWall(5, 3, vec3(-40, -2, 50),0, false);
    enviro->generateWall(5, 3, vec3(-30, -2, 50),0, false);
    enviro->generateWall(5, 4, vec3(-20, -2, 50),0, false);
    enviro->generateWall(5, 4, vec3(-10, -2, 50),0, false);
    enviro->generateWall(5, 3, vec3(0, -2, 50),0, false);
    enviro->generateWall(5, 3, vec3(10, -2, 50),0, false);
    enviro->generateWall(5, 3, vec3(20, -2, 50),0, false);
    enviro->generateWall(5, 3, vec3(30, -2, 50),0, false);
    enviro->generateWall(5, 3, vec3(40, -2, 50),0, false);
    //                      3 
    enviro->generateWall(5, 3, vec3(-60, -2, -50), 90, false);
    enviro->generateWall(5, 3, vec3(-60, -2, -40), 90, false);
    enviro->generateWall(5, 3, vec3(-60, -2, -30), 90, false);
    enviro->generateWall(5, 3, vec3(-60, -2, -20), 90, false);
    enviro->generateWall(5, 3, vec3(-60, -2, -10), 90, false);
    enviro->generateWall(5, 4, vec3(-60, -2, 0), 90, false);
    enviro->generateWall(5, 4, vec3(-60, -2, 10), 90, false);
    enviro->generateWall(5, 3, vec3(-60, -2, 20), 90, false);
    enviro->generateWall(5, 3, vec3(-60, -2, 30), 90, false);
    enviro->generateWall(5, 3, vec3(-60, -2, 40), 90, false);
    //                      3 
    enviro->generateWall(5, 3, vec3(40, -2, -50), 90, false);
    enviro->generateWall(5, 3, vec3(40, -2, -40), 90, false);
    enviro->generateWall(5, 3, vec3(40, -2, -30), 90, false);
    enviro->generateWall(5, 4, vec3(40, -2, -20), 90, false);
    enviro->generateWall(5, 3, vec3(40, -2, -10), 90, false);
    enviro->generateWall(5, 4, vec3(40, -2, 0), 90, false);
    enviro->generateWall(5, 3, vec3(40, -2, 10), 90, false);
    enviro->generateWall(5, 4, vec3(40, -2, 20), 90, false);
    enviro->generateWall(5, 3, vec3(40, -2, 30), 90, false);
    enviro->generateWall(5, 3, vec3(40, -2, 40), 90, false);


  }

  void SceneCastle::GenerateRandomWalls(EnvironmentManager* enviro, int quantity)
  {
    for (int i = 0; i < quantity; i++)
    {
      int r = rand() % 2;
      enviro->generateRandomWall(5, 3+r);
    }
  }

  void SceneCastle::GenerateRandomBuildings(EnvironmentManager* enviro, int quantity)
  {
    enviro->BLOCK_SIZE = vec3(0.5,0.5,0.5);
    for (size_t i = 0; i < quantity; i++)
    {
      int r = rand() % 5; // generate a coordinate +- half the world size
      enviro->generateRandomBuilding(3 + r, 3 + r, 15 + r);
    }
    enviro->BLOCK_SIZE = vec3(1,1,1);

  }

  // magic numbers fixed the issue
  void SceneCastle::GenerateBoundry(EnvironmentManager* enviro, float height)
  {
    float size = (enviro->getWorldSize() + 2 * height)/2.0f;

    int blockid = ObjectFactory::CreateBox(size + 2 * height, height, height, 0, height,  0, -size, MT_WALL_BRICK); // back  -Z
    GameObject* obj = ObjectManager::GetObjectByID(blockid);
    ModelComponent* model = obj->GetComponent<ModelComponent>();
    size_t id = model->GetInstance();
    Rendering::IModel* m = ModelManager::GetModel(MT_WALL_BRICK);
    m->SetColor(id, 0.3f, 0.3f, 0.3f);

    blockid = ObjectFactory::CreateBox(size + 3 * height,  height, height, 0,  0, 0, 113.5f, MT_WALL_BRICK); // front +Z
    obj = ObjectManager::GetObjectByID(blockid);
    model = obj->GetComponent<ModelComponent>();
    id = model->GetInstance();
    m->SetColor(id, 0.3f, 0.3f, 0.3f);

    blockid = ObjectFactory::CreateBox(height, height, size + height, 0,  148, 0,  0, MT_WALL_BRICK); // right +X
    obj = ObjectManager::GetObjectByID(blockid);
    model = obj->GetComponent<ModelComponent>();
    id = model->GetInstance();
    m->SetColor(id, 0.3f, 0.3f, 0.3f);

    blockid = ObjectFactory::CreateBox(height, height, size + height, 0, -size, 0,  0, MT_WALL_BRICK); // left  -X
    obj = ObjectManager::GetObjectByID(blockid);
    model = obj->GetComponent<ModelComponent>();
    id = model->GetInstance();
    m->SetColor(id, 0.3f, 0.3f, 0.3f);
  }
}