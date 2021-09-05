#include "pch.h"
#include "SceneDigipen.h"
#include "App.h"
#include "ObjectManager.h"
#include "ModelManager.h"
#include "Transform.h"

void PAEngine::SceneDigipen::SceneInit()
{
    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "Digi Logo Init");

    EventSystem::SendEvent(ET_LOGOONOFF, (void*)nullptr, sizeof(nullptr));

   //// if (!GetCameraManagement()->isCameraOn(0))
    //    GetCameraManagement()->addCamera(vec3(0.f));
}

void PAEngine::SceneDigipen::SceneAcquireInput(TypeCommandActiveMap activeKeyMap, Gamepad* pGamepad)
{
}

void PAEngine::SceneDigipen::SceneUpdate(const double deltaTime)
{
    timeToDisplay -= deltaTime;

    if (timeToDisplay <= 0)
    {
        PAEngine::App::globalApp->ChangeScene("menu");
    }
}

void PAEngine::SceneDigipen::ScenePhysicsUpdate(const double deltaTime)
{
}

void PAEngine::SceneDigipen::SceneRender(const double farseer)
{
}

void PAEngine::SceneDigipen::SceneShutdown()
{
  EventSystem::SendEvent(ET_LOGOONOFF, (void*)nullptr, sizeof(nullptr));
}
