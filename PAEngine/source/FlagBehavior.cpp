/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#include "pch.h"
#include "FlagBehavior.h"
#include "Transform.h" 
#include "ObjectManager.h"
#include "Grid.h"
#include "ChildTransform.h"
#include "EnvironmentManager.h"
#include "Physics.h"
#include "PlayerWin.h"
#include "BulletBehavior.h"
#include "PlayerBehavior.h"
#include "App.h"
#include "ModelComponent.h"
#include "ModelManager.h"
#include "FlagBehavStateDefault.h"
#include "PlayerController.h"
#include "AstarBehavior.h"
#include "PAGui.h"
#include "PAGuiWidgets.h"

void BulletCollidesWithFlag_CB(unsigned myID, unsigned otherID, const std::string& target)
{
  FlagBehavior* behav = ObjectManager::GetObjectByID(myID)->GetComponent<FlagBehavior>();
  float position[3];
  ObjectManager::GetObjectByID(myID)->GetComponent<Transform>()->GetPosition(position);
  ObjectManager::GetObjectByID(otherID)->GetComponent<Transform>()->GetPosition(position);
  if (behav->GetPlayer() > 0)
  {
    return;
  }
  EventSystem::SendEvent(ET_SOUND_OS_SHOTHIT, nullptr, 0);
  //BulletBehavior* bulletbehav = ObjectManager::GetObjectByID(otherID)->GetComponent<BulletBehavior>();
  behav->DecrementHP();
  if (behav->GetHP() < 0)
  {
    EventSystem::SendEvent(ET_SOUND_OS_TURRETCAPTURE, nullptr, 0);
    PlayerBehavior* playerbehav = ObjectManager::GetObjectByID(otherID)->GetComponent<PlayerBehavior>();
    if (playerbehav)
    {
      behav->SetPlayer(playerbehav->GetPlayerID());
      playerbehav->hasFlag = true;
    }
    //PlayerWin::globalWin->SetWinner(0);
  }
}
FlagBehavior::FlagBehavior()
  : gamepad_(nullptr), forces_(nullptr), indicatorID(-1) {
  AddBehaviorState(new FlagBehavStateDefault(this));

}

FlagBehavior::~FlagBehavior() {
  indicatorID = -1;
}

void FlagBehavior::Create(int parentID) {
  ChangeState(FBS_NEUTRAL);
  GameObject* gameObj = ObjectManager::GetObjectsByName("flag")[0];
  //gameObj->GetComponent<Physics>()->box->addCollisionCallback((OnCollision*)&BulletCollidesWithFlag_CB, "player");
}

void FlagBehavior::Ending()
{

  EnvironmentManager* enviro = EnvironmentManager::getEnvironment();
  const std::vector<int>& ground = EnvironmentManager::getEnvironment()->getGround();
  int total = enviro->gridAmount * enviro->gridAmount;
  
  EventSystem::SendEvent(ET_SOUND_L_END_SCORECOUNTER, nullptr, 0);
  if (count)
  {
    ++timer;
    if (timer > 0 && timer % (speed / 2) == 0 && index < total)
    {
      GetCameraManager()->addMegaMap();
      int color = ObjectManager::GetObjectByID(ground[index])->GetComponent<AstarBehavior>()->ToOwnerColor();

      while (color == -1 && index < total - 1)
      {
        ++index;
        color = ObjectManager::GetObjectByID(ground[index])->GetComponent<AstarBehavior>()->ToOwnerColor();
      }
      ++index;

      switch (color)
      {
      case 0:
        ++red;
        break;
      case 1:
        ++blue;
        break;
      case 2:
        ++yellow;
        break;
      case 3:
        ++green;
        break;
      default:
        break;
      }

    }

    if (index >= total && !fin2)
    {
      PlayerWin::globalWin->SetWinner(getMax());
      fin2 = true;
    }
  }
  else
  {
    if (black)
    {
      ++timer;
    }
    else
    {
      --timer;
    }

    if (timer % speed != 0)
    {
      return;
    }
    for (int i = 0; i < total; ++i)
    {
      int row = i % enviro->gridAmount;
      int col = i / enviro->gridAmount;
      if (timer / speed == row + col)
      {
        ObjectManager::GetObjectByID(ground[i])->GetComponent<AstarBehavior>()->ToBlackWhite(black);
      }
    }
    if (timer > (enviro->gridAmount) * speed * 2)
    {
      black = false;
    }
    if (timer < -speed)
    {
      count = true;
      timer = 0;
    }

    

    EventSystem::SendEvent(ET_SOUND_L_START_SCORECOUNTER, &winTime, sizeof(float));
  }
}


void FlagBehavior::Destroy() {
  gamepad_ = nullptr;
  Behavior::Shutdown();

}

void FlagBehavior::ColorUpdate()
{
  if (PAEngine::App::globalApp->isPaused)
  {
    return;
  }
  if (menuGUI || pressGUI)
  {

  }
  else
  {
    --winTime;
  }

  EnvironmentManager* enviro = EnvironmentManager::getEnvironment();
  int total = enviro->gridAmount * enviro->gridAmount;
  redPercent = (float)red / (float)total * 100.f;
  bluePercent = (float)blue / (float)total * 100.f;
  yellowPercent = (float)yellow / (float)total * 100.f;
  greenPercent = (float)green / (float)total * 100.f;
  grayPercent = 100.f - redPercent - bluePercent - yellowPercent - greenPercent;

  if (winTime < 0 && !fin)
  {
    fin = true;
    red = 0;
    blue = 0;
    yellow = 0;
    green = 0;

    EventSystem::SendEvent(ET_UIONOFF, (void*)nullptr, sizeof(nullptr));
    EventSystem::SendEvent(ET_FINISHONOFF, (void*)nullptr, sizeof(nullptr));
    //PlayerWin::globalWin->SetWinner(getMax());
  }
  if (fin3)
  {
    Ending();
  }
}

int FlagBehavior::getMax()
{
  if (red > blue)
  {
    if (red > yellow)
    {
      if (red > green)
      {
        return 0;
      }
      else
      {
        return 3;
      }
    }
    else
    {
      if (yellow > green)
      {
        return 2;
      }
      else
      {
        return 3;
      }
    }
  }
  else
  {
    if (blue > yellow)
    {
      if (blue > green)
      {
        return 1;
      }
      else
      {
        return 3;
      }
    }
    else
    {
      if (yellow > green)
      {
        return 2;
      }
      else
      {
        return 3;
      }
    }
  }
  return -1;
}

void FlagBehavior::ChangeColors(int attacker, int attacked)
{
  int changeCount = 0;
  switch (attacked)
  {
  case 0:
    changeCount = red / 4;
    red -= changeCount;
    oldcolor[0] = 100;
    break;
  case 1:
    changeCount = blue / 4;
    blue -= changeCount;
    oldcolor[1] = 100;
    break;
  case 2:
    changeCount = yellow / 4;
    yellow -= changeCount;
    oldcolor[2] = 100;
    break;
  case 3:
    changeCount = green / 4;
    green -= changeCount;
    oldcolor[3] = 100;
    break;
  default:
    break;
  }

  vec3 color;
  switch (attacker)
  {
  case 0:
    color.x = 1;
    color.y = 0;
    color.z = 0;
    red += changeCount;
    break;
  case 1:
    color.x = 0;
    color.y = 0;
    color.z = 1;
    blue += changeCount;
    break;
  case 2:
    color.x = 1;
    color.y = 1;
    color.z = 0;
    green += changeCount;
    break;
  case 3:
    color.x = 0;
    color.y = 1;
    color.z = 0;
    blue += changeCount;
    break;
  default:
    break;
  }


  int counter = 0;
  const std::vector<int>& ground = EnvironmentManager::getEnvironment()->getGround();
  for (int i = 0; i < ground.size(); ++i)
  {
    GameObject* groundObj = ObjectManager::GetObjectByID(ground[i]);
    AstarBehavior* groundBehav = groundObj->GetComponent<AstarBehavior>();

    if (groundBehav == nullptr)
      continue;

    if (groundBehav->owner == attacked)
    {
      groundBehav->owner = attacker;
      vec3 color;
      switch (attacker)
      {
      case 0:
        color.x = 1;
        color.y = 0;
        color.z = 0;
        break;
      case 1:
        color.x = 0;
        color.y = 0;
        color.z = 1;
        break;
      case 2:
        color.x = 1;
        color.y = 1;
        color.z = 0;
        break;
      case 3:
        color.x = 0;
        color.y = 1;
        color.z = 0;
        break;
      default:
        break;
      }


      ModelComponent* pModel = groundObj->GetComponent<ModelComponent>();
      if (pModel != nullptr)
      {
        ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
          color.x, color.y, color.z);
      }
      
      
      ++counter;
      if (counter >= changeCount)
      {
        break;
      }
    }
  }

}

void FlagBehavior::flagUpdate()
{
  //  update Flags movement from gamepad
  GameObject* flag = ObjectManager::GetObjectByID(GetParent());
  //GameObject* player1 = ObjectManager::GetObjectByID(2);
  //GameObject* player2 = ObjectManager::GetObjectByID(5);
  //GameObject* player3 = ObjectManager::GetObjectByID(8);
  //GameObject* player4 = ObjectManager::GetObjectByID(11);

  Physics* physics = flag->GetComponent<Physics>();
  physics->box->rigidbody->setAngularVelocity(btVector3(0.f, 1.f, 0.f));
  physics->box->rigidbody->setLinearVelocity(btVector3(0.f, 0.f, 0.f));
  Transform* transform = flag->GetComponent<Transform>();
  float posX = transform->GetPositionX();
  float posZ = transform->GetPositionZ();
  float posY = transform->GetPositionY();

  if (player == -1)
  {
    physics->Teleport(0, 0 - posY, 0);
    return;
  }

  /*
  BoxCollider* collider = physics->box;
  if (collider->isColliding("bullet"))
  {
    --HP;
    if (HP < 0)
    {
      player = 1;
    }
  }
  */
  Transform* playerTransform = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(player))->GetComponent<Transform>();
  float playerX = playerTransform->GetPositionX();
  float playerZ = playerTransform->GetPositionZ();


  float dispX = playerX - posX;
  float dispZ = playerZ - posZ;
  float dispY = 2 - posY;

  // makes theh flag follow the assigned player
  physics->Teleport(dispX, dispY, dispZ);
  
  // makes the indicator (flag's children) follow the flag (parent)
  std::unordered_set<int> childObjIDs = ObjectManager::GetChildObjects(this->GetParentID());
  for (auto childObj : childObjIDs)
  {
    Physics* childPhys = ObjectManager::GetObjectByID(childObj)->GetComponent<Physics>();
    childPhys->Teleport(dispX, dispY, dispZ);
  }

  if(!PAEngine::App::globalApp->isPaused)
  {
    IncrementTime(player);
  }
  
  if (GetTime(player) > winTime && !fin && finishGame)
  {
    fin = true;
    PlayerWin::globalWin->SetWinner(player);
  }

  if (fin)
  {
    Ending();
  }
  
}


void FlagBehavior::DecrementHP()
{
  --HP;
}
int FlagBehavior::GetHP()
{
  return HP;
}
void FlagBehavior::SetPlayer(int p)
{
  player = p;
}
int FlagBehavior::GetPlayer()
{
  return player;
}

int FlagBehavior::GetTime(int player)
{
  if (player == 0)
  {
    return time1;
  }
  if (player == 1)
  {
    return time2;
  }
  if (player == 2)
  {
    return time3;
  }
  if (player == 3)
  {
    return time4;
  }
  return 0;
}

void FlagBehavior::IncrementTime(int player)
{
  if (player == 0)
  {
    ++time1;
  }
  if (player == 1)
  {
    ++time2;
  }
  if (player == 2)
  {
    ++time3;
  }
  if (player == 3)
  {
    ++time4;
  }
  //return 0;
}

int FlagBehavior::GetWinTime()
{
  return winTime;
}
