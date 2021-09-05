#include "pch.h"
#include "ScenePlayerTest.h"
#include "ObjectManager.h"
#include "ObjectFactory.h"

namespace PAEngine
{
  static int objectid;
  void ScenePlayerTest::SceneInit()
  {
    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "ScenePlayerTest Init");

    // create ground
    int ground = ObjectFactory::CreateGround();

    // creating players
    playerController_.Init();


    //destructable wall
#ifndef _DEBUG
    for (float i = 0.5; i < 10; i++)
    {
      for (float j = 0.5; j < 10; j++)
      {
        int block = ObjectFactory::CreateBox(1, 1, 1, 0.5, i - 10 + i + (int(i) % 2), j + j, -20);
      }
    }
#endif

  }

  void ScenePlayerTest::SceneAcquireInput(TypeCommandActiveMap activeKeyMap, Gamepad* pGamepad)
  {
  }

  void ScenePlayerTest::SceneUpdate(const double deltaTime)
  {
    playerController_.Update(deltaTime);
  }

  void ScenePlayerTest::ScenePhysicsUpdate(const double deltaTime)
  {

  }

  void ScenePlayerTest::SceneRender(const double farseer) {
  }

  void ScenePlayerTest::SceneShutdown()
  {
    playerController_.Shutdown();
    ObjectManager::ClearObjects();
  }
}