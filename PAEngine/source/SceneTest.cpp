/**
 * @file SceneTest.cpp
 *
 * @brief It's an implementation of SceneTest.h
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include "SceneTest.h"
#include "EventSystem.h"
#include "ObjectManager.h"
#include "Transform.h"
#include "ChildTransform.h"
#include "ModelComponent.h"
#include "ObjectFactory.h"
#include "EnvironmentManager.h"
#include "App.h"


namespace PAEngine
{
	void SceneTest::SceneInit()
	{	
		Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "SceneTest Init");

    /////////////////////
    // Keyboard and Mouse

    /////////
    // XInput
    playerController_.Init();

    // nothing yet

    ThisIsAPrivateFunctionOfSceneTest();

    // tank
    objectid = ObjectFactory::CreateBox(1, 1, 1, 1, 0,0,0);
    int tankChild = ObjectFactory::CreateChildBox(1, 1, 1, 1, 1, 1);
    ObjectManager::MakeParent(tankChild, objectid);

    //GetCameraManagement()->attachToObject(objectid, 0, vec3(0, 5, 10));

    // create ground
    EnvironmentManager* enviro = EnvironmentManager::getEnvironment();
    enviro->generateGround(200, 19);

    int flag = ObjectFactory::CreateFlag();
    int astar = ObjectFactory::CreateAstar();

#ifndef _DEBUG
#endif

  }

	void SceneTest::SceneAcquireInput(TypeCommandActiveMap activeKeyMap, Gamepad* pGamepad)
	{
		// act on user input
		for (auto x : activeKeyMap)
		{
      Physics* phys = ObjectManager::GetObjectByID(objectid)->GetComponent<Physics>();
      Transform* trans = ObjectManager::GetObjectByID(objectid)->GetComponent<Transform>();
			switch (x.first)
			{
        //case EventType::ET_INPUT_KEY_CHANGING:
        //EventSystem::SendEvent(ET_SOUND_OS_PLAYERSHOOT, (void*)0, sizeof(void*));
        //ObjectFactory::CreateBullet(objectid, trans->GetPositionX(), trans->GetPositionY(), trans->GetPositionZ() - 0.5,
        //                            0, .1, -1, 10);
        break;

        case EventType::ET_INPUT_FORWARD:
        phys->SetImpulse(0.f,0.f ,-1.f);
        break;

        case EventType::ET_INPUT_BACKWARD:
        phys->SetImpulse(0.f,0.f ,1.f);
        break;

        case EventType::ET_INPUT_LEFT:
        phys->SetImpulse(1.f,0.f ,0.f);
        break;

        case EventType::ET_INPUT_RIGHT:
        phys->SetImpulse(-1.f,0.f ,0.f);
        break;
			}
		}
	}

  void SceneTest::SceneUpdate(const double deltaTime)
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

  void SceneTest::ScenePhysicsUpdate(const double deltaTime)
  {

  }

	void SceneTest::SceneRender(const double farseer){
	}

	void SceneTest::SceneShutdown()
	{
    playerController_.Shutdown();
    ObjectManager::ClearObjects();
	}

  void SceneTest::ThisIsAPrivateFunctionOfSceneTest()
  {
    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "This Is A Private Function Of Scene Test");
  }

  void KeyChanging(void* data)
  {
    //std::unordered_map<EventType, GameCommand*>* pKeyMap = (std::unordered_map<EventType, GameCommand*>*)data;

    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "Swaping key funtion needs to newly implement.");

    //// swap pointers
    //GameCommand* pTemp = pKeyMap->find(EventType::ET_INPUT_RIGHT)->second;
    //pKeyMap->find(EventType::ET_INPUT_RIGHT)->second = pKeyMap->find(EventType::ET_INPUT_LEFT)->second;
    //pKeyMap->find(EventType::ET_INPUT_LEFT)->second = pTemp;
  }
    }