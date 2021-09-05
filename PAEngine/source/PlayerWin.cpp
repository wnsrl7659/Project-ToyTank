#include "pch.h"
#include "PlayerWin.h"
#include "PlayerController.h"
#include "ObjectManager.h"
#include "Physics.h"
#include "Transform.h"
#include "PAGuiWidgets.h"

PlayerWin* PlayerWin::globalWin = nullptr;

PlayerWin::PlayerWin()
{
  if (globalWin != nullptr)
  {
    return;
  }
  globalWin = this;
}

PlayerWin::~PlayerWin() 
{

}

void PlayerWin::SetWinner(int n)
{
  if (winner == -1)
  {
    EventSystem::SendEvent(ET_WINONOFF, (void*)nullptr, sizeof(nullptr));
    GUIwinner = n;
    winner = n;
    //teleport

    for (int i = 0; i < 4; i++)
    {
      if (winner != i)
      {
        int playerobjID = PlayerController::GetObjectIDFromPlayerID(i);
        vec3 playerInitPos = PlayerController::GetObjectInitPosFromPlayerID(i);
        if (playerobjID == -1)
        {
          continue;
        }
        Physics* trans = ObjectManager::GetObjectByID(playerobjID)->GetComponent<Physics>();
        std::vector<float>& matrix = trans->GetMatrix();

        trans->Teleport(-matrix[12], -matrix[13], -matrix[14]);
        trans->Teleport(3.0f * i, -10.0f, 0.0f);
        trans->box->rigidbody->setLinearVelocity(btVector3(0, 0, 0));
        trans->box->rigidbody->setAngularVelocity(btVector3(0, 0, 0));
      }
    }
  }

  if (n == -1)
  {
    GUIwinner = -1;
    winner = -1;
  }
}

const int PlayerWin::GetWinner()
{
  return winner;
}
