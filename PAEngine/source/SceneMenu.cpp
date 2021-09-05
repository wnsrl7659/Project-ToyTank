#include "pch.h"
#include "SceneMenu.h"
#include "EventSystem.h"
#include "ObjectManager.h"
#include "Transform.h"
#include "ChildTransform.h"
#include "ModelComponent.h"
#include "ObjectFactory.h"
#include "EnvironmentManager.h"
#include "PAGuiWidgets.h"
#include "PAGui.h"
#include "App.h"

namespace PAEngine
{
    void SceneMenu::SceneInit()
    {
      Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "menu Init");

      EventSystem::SendEvent(ET_PRESSONOFF, (void*)nullptr, sizeof(nullptr));


      /////////
      // XInput
      playerController_.Init();

      EnvironmentManager* enviro = EnvironmentManager::getEnvironment();
      enviro->generateGround(200, 19);

      int flag = ObjectFactory::CreateFlag();
      int astar = ObjectFactory::CreateAstar();

      EventSystem::SendEvent(ET_SOUND_L_START_MENUMUSIC, nullptr, 0);
    }

    void SceneMenu::SceneAcquireInput(TypeCommandActiveMap activeKeyMap, Gamepad* pGamepad)
    {
      // act on user input
      for (auto x : activeKeyMap)
      {
        if (!pressGUI)
        {
            switch (x.first)
            {
            case EventType::ET_INPUT_X:
                EventSystem::SendEvent(ET_GUIONOFF, (void*)nullptr, sizeof(nullptr));
                break;

            case EventType::ET_INPUT_ML:
                if (confirmGUI)
                {
                  EventSystem::SendEvent(ET_SOUND_OS_BUTTONNAVIGATION, nullptr, 0);

                    if (confirmtab > 0)
                    {
                        confirmtab--;
                    }
                }
                break;
            case EventType::ET_INPUT_MR:
                if (confirmGUI)
                {
                  EventSystem::SendEvent(ET_SOUND_OS_BUTTONNAVIGATION, nullptr, 0);

                    if (confirmtab < 1)
                    {
                        confirmtab++;
                    }
                }
                break;
            case EventType::ET_INPUT_MU:
                if (menustate == 0 && !confirmGUI)
                {
                  EventSystem::SendEvent(ET_SOUND_OS_BUTTONNAVIGATION, nullptr, 0);

                    if (menutab > 0)
                    {
                        menutab--;
                    }
                }
                break;
            case EventType::ET_INPUT_MD:
                if (menustate == 0 && !confirmGUI)
                {
                  EventSystem::SendEvent(ET_SOUND_OS_BUTTONNAVIGATION, nullptr, 0);
                    if (menutab < 3)
                    {
                        menutab++;
                    }
                }
                break;
            case EventType::ET_INPUT_A:
                if (!confirmGUI)
                {
                  EventSystem::SendEvent(ET_SOUND_OS_BUTTONSELECT, nullptr, 0);
                    if (menustate == 0)
                    {
                        if (menutab == 0)
                        {
                            PAEngine::App::globalApp->ChangeScene("castle");
                            EventSystem::SendEvent(ET_UIONOFF, (void*)nullptr, sizeof(nullptr));
                        }
                        else if (menutab == 1)
                        {
                            menustate = 1;
                        }
                        else if (menutab == 2)
                        {
                            menustate = 2;
                        }
                        else if (menutab == 3)
                        {
                            EventSystem::SendEvent(ET_CONFIRMONOFF, (void*)nullptr, sizeof(nullptr));
                        }
                    }
                    else if (menustate == 1)
                    {
                        menustate = 0;
                    }
                    else if (menustate == 2)
                    {
                        menustate = 0;
                    }
                    break;
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
            }
        }
        else
        {
          switch (x.first)
          {
          case EventType::ET_INPUT_A:
            EventSystem::SendEvent(ET_MENUONOFF, (void*)nullptr, sizeof(nullptr));
            EventSystem::SendEvent(ET_PRESSONOFF, (void*)nullptr, sizeof(nullptr));
            break;
          }
        }
      }
    }

    void SceneMenu::SceneUpdate(const double deltaTime)
    {
    }

    void SceneMenu::ScenePhysicsUpdate(const double deltaTime)
    {

    }

    void SceneMenu::SceneRender(const double farseer) {
    }

    void SceneMenu::SceneShutdown()
    {
      EventSystem::SendEvent(ET_MENUONOFF, (void*)nullptr, sizeof(nullptr));
      playerController_.Shutdown();
      ObjectManager::ClearObjects();
      EventSystem::SendEvent(ET_SOUND_L_END_MENUMUSIC, nullptr, 0);

    }
}