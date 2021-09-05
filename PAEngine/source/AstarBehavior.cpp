/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#include "pch.h"
#include "AstarBehavior.h"
#include "Transform.h" 
#include "ObjectManager.h"
#include "Grid.h"
#include "ChildTransform.h"
#include "EnvironmentManager.h"
#include "Physics.h"
#include "PlayerWin.h"
#include "BulletBehavior.h"
#include "PlayerBehavior.h"
#include "Grid.h"
#include "GameObject.h"
#include "ObjectFactory.h"
#include "ModelManager.h"
#include "AstarBehavStateDefault.h"
#include "FlagBehavior.h"
#include "PlayerController.h"
#include <chrono>
#include "ParticleEmitter.h"

void BulletCollidesWithAstar_CB(unsigned myID, unsigned otherID, const std::string& bullet)
{

}
AstarBehavior::AstarBehavior()
    : gamepad_(nullptr), forces_(nullptr) {
    AddBehaviorState(new AstarBehavStateDefault(this));

}

AstarBehavior::~AstarBehavior() {
}

void AstarBehavior::Create(int parentID) {
    ChangeState(FBS_NEUTRAL);
    parentID_ = parentID;
}

void AstarBehavior::Update(float dt)
{
    Behavior::Update(dt);
    if (ParticleTimer > 0)
    {
        ParticleTimer -= dt;
    }
    else
    {
        GameObject* astar = ObjectManager::GetObjectByID(GetParent());

        if (astar->GetName() == "ground")
        {
            astar->GetComponent<ParticleEmitter>(0)->SetActive(false);
            astar->GetComponent<ParticleEmitter>(1)->SetActive(false);
        }
    }
}

void AstarBehavior::Destroy() {
    gamepad_ = nullptr;
    Behavior::Shutdown();
}


void AstarBehavior::ToBlackWhite(bool b)
{
    if (!this)
        return;

    const std::vector<int>& ground = EnvironmentManager::getEnvironment()->getGround();
    GameObject* groundObj = ObjectManager::GetObjectByID(ground[index]);
    ModelComponent* pModel = groundObj->GetComponent<ModelComponent>();
    if (pModel != nullptr)
    {
        if (b)
        {
            color = owner;
            owner = -1;
            ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
                0, 0, 0);
        }
        else
        {
            ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
                1, 1, 1);
        }
    }
}

void AstarBehavior::ToWhite()
{
    const std::vector<int>& ground = EnvironmentManager::getEnvironment()->getGround();
    GameObject* groundObj = ObjectManager::GetObjectByID(ground[index]);
    ModelComponent* pModel = groundObj->GetComponent<ModelComponent>();
    if (pModel != nullptr)
    {
        ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
            1, 1, 1);
    }
}

int AstarBehavior::ToOwnerColor()
{
    if (!this)
        return -1;

    owner = color;
    if (owner < 0)
        return -1;

    GameObject* player = ObjectManager::GetObjectByID(PlayerController::GetObjectIDFromPlayerID(owner));
    vec3 color = player->GetComponent<PlayerBehavior>()->colorOrigin;
    const std::vector<int>& ground = EnvironmentManager::getEnvironment()->getGround();
    GameObject* groundObj = ObjectManager::GetObjectByID(ground[index]);
    ModelComponent* pModel = groundObj->GetComponent<ModelComponent>();
    if (pModel != nullptr)
    {
        ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
            color.x, color.y, color.z);
    }
    return owner;
}



void AstarBehavior::groundUpdate()
{
    if (hit < 0)
    {
        return;
    }
    GameObject* player = ObjectManager::GetObjectByID(owner);

    Transform* playerTransform = (player)->GetComponent<Transform>();
    float playerX = playerTransform->GetPositionX();
    float playerZ = playerTransform->GetPositionZ();

    const std::vector<int>& ground = EnvironmentManager::getEnvironment()->getGround();
    GameObject* groundObj = ObjectManager::GetObjectByID(ground[index]);

    Transform* groundTransform = (groundObj)->GetComponent<Transform>();
    float groundX = groundTransform->GetPositionX();
    float groundZ = groundTransform->GetPositionZ();

    float distance = (playerX - groundX) * (playerX - groundX) + (playerZ - groundZ) * (playerZ - groundZ);
    EnvironmentManager* enviro = EnvironmentManager::getEnvironment();
    float row = enviro->gridAmount;
    float worldWidth = enviro->getWorldSize();
    float width = worldWidth / row;

    printf("%i: %f, %f\n", index, width * width / 2, distance);

    if (distance > width * width / 2)
    {
        hit = -1;
        return;
    }
    else if (distance < width * width / 3)
    {
        GameObject* flag = ObjectManager::GetObjectsByName("flag")[0];
        FlagBehavior* flagbehav = flag->GetComponent<FlagBehavior>();
        int id = player->GetComponent<PlayerBehavior>()->GetPlayerID();
        switch (id)
        {
        case 0:
            ++flagbehav->red;
            break;
        case 1:
            ++flagbehav->blue;
            break;
        case 2:
            ++flagbehav->yellow;
            break;
        case 3:
            ++flagbehav->green;
            break;
        default:
            return;
        }

        int oid = owner;
        switch (oid)
        {
        case 0:
            --flagbehav->red;
            break;
        case 1:
            --flagbehav->blue;
            break;
        case 2:
            --flagbehav->yellow;
            break;
        case 3:
            --flagbehav->green;
            break;
        default:
            break;
        }

        owner = id;

        vec3 color = player->GetComponent<PlayerBehavior>()->colorOrigin;
        ModelComponent* pModel = groundObj->GetComponent<ModelComponent>();
        if (pModel != nullptr)
        {
            ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
                color.x, color.y, color.z);
        }


        hit = -1;
    }



}

void AstarBehavior::ChangeColor()
{
  ModelComponent* pModel = ObjectManager::GetObjectByID(GetParent())->GetComponent<ModelComponent>();
  currentcolor += (targetcolor - oldcolor) * 0.3f;
  if (pModel != nullptr)
  {
    ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
      currentcolor.x, currentcolor.y, currentcolor.z);
  }

  float distance = (targetcolor.x - currentcolor.x) * (targetcolor.x - currentcolor.x) + (targetcolor.y - currentcolor.y) * (targetcolor.y - currentcolor.y) + (targetcolor.z - currentcolor.z) * (targetcolor.z - currentcolor.z);
  printf("%f\n", distance);
  if (distance > 20.f)
  {
    currentcolor = targetcolor;
    if (pModel != nullptr)
    {
      ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
        currentcolor.x, currentcolor.y, currentcolor.z);
    }
    changingColor = false;
  }
}

int AstarBehavior::getIndex(float xPos, float zPos)
{
    EnvironmentManager* enviro = EnvironmentManager::getEnvironment();
    float row = enviro->gridAmount;
    float worldWidth = enviro->getWorldSize();
    float width = worldWidth / row;
    float r = (xPos + worldWidth / 2) / width;
    float c = (zPos + worldWidth / 2) / width;
    return (int)r + row * (int)c;
}


void AstarBehavior::astarColorUpdate()
{
  if (changingColor)
  {
    ChangeColor();
  }
}


void AstarBehavior::astarUpdate()
{
    //  update Astars movement from gamepad
    GameObject* astar = ObjectManager::GetObjectByID(GetParent());
    GameObject* player1 = ObjectManager::GetObjectsByName("player")[0];
    //GameObject* player2 = ObjectManager::GetObjectByID(5);
    //GameObject* player3 = ObjectManager::GetObjectByID(8);
    //GameObject* player4 = ObjectManager::GetObjectByID(11);

    Physics* physics = astar->GetComponent<Physics>();
    Transform* transform = astar->GetComponent<Transform>();
    float posX = transform->GetPositionX();
    float posZ = transform->GetPositionZ();

    Transform* playerTransform = player1->GetComponent<Transform>();
    float playerX = playerTransform->GetPositionX();
    float playerZ = playerTransform->GetPositionZ();

    //float dispX = playerX - posX;
    //float dispZ = playerZ - posZ;

    //float distance = dispX * dispX + dispZ * dispZ;
    /*if (distance > 0 && distance < 2000)
    {
      float speed = 0.1f;
      distance = speed / sqrt(distance);
      physics->Teleport(dispX * distance, 0, dispZ * distance);
    }*/


    //  Get my position

    //  Get my node index
    Grid& grid = EnvironmentManager::getGrid();


    if (!set)
    {
        if (!initiate)
        {
            myIndex = grid.getNodeIndex(posX, posZ);
        }

        myNode = grid.getNode(myIndex);

        //  Get target position
        //  Get target node index
        targetIndex = grid.getNodeIndex(playerX, playerZ);

        //  Set my node's distance to 0
        myNode->SetDistance(0);
        findInitial = true;
        set = true;
        if (myIndex == targetIndex || !grid.Difference(myIndex, targetIndex))
        {
            set = false;
            findInitial = false;
            return;
        }
    }

    if (findInitial)
    {
        initiate = true;
        //  Create a priority queue

        //  for each node in grid.nodes set heuristic
        grid.SetHeuristicsAll(targetIndex);
        //  Add my node's neighbor to the PQ and set InQueue
        std::vector<int> neighbors = myNode->GetNeighbors();
        for (std::vector<int>::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
        {
            Grid::Node* neigh = grid.getNode(*it);
            neigh->SetParent(myIndex);
            neigh->Enqueue(true);
            neigh->SetDistance(1);
            std::pair<int, int> neighPair(-neigh->GetDistance() - neigh->GetHeuristic(), neigh->GetIndex());
            PQ.push(neighPair);
        }
        found = false;
        findInitial = false;
    }

    //  while PQ is not empty
    if (!found)
    {
        //    get PQ.top
        std::pair<int, int> nextPair = PQ.top();
        //    if top is goal, found = true, break
        if (nextPair.second == targetIndex)
        {
            found = true;
        }
        else
        {
            //    remove top from PQ
            PQ.pop();
            Grid::Node* next = grid.getNode(nextPair.second);
            next->Enqueue(false);
            //    Get the neighbors of top
            std::vector<int> neighbors2 = next->GetNeighbors();
            int topDistance = next->GetDistance();
            int topIndex = next->GetIndex();
            //    for each neighbor n
            for (std::vector<int>::iterator it = neighbors2.begin(); it != neighbors2.end(); ++it)
            {
                Grid::Node* n = grid.getNode(*it);
                //      if not wall
                if (n->IsWall())
                {
                    continue;
                }
                int newDist = topDistance + 1;
                //        if newDist < n.dist
                if (n->GetDistance() > newDist)
                {
                    //n.dist = newDist
                    n->SetDistance(newDist);
                    //n.parent = top
                    n->SetParent(topIndex);
                    //n.enqueue (if not already in queue)
                    if (!n->IsInQueue())
                    {
                        n->Enqueue(true);
                        std::pair<int, int> nextNode(-newDist - n->GetHeuristic(), n->GetIndex());
                        PQ.push(nextNode);
                    }
                }
            }
        }

    }
    //
    //  if found
    //    backtrack from goal until my index
    if (found && !moving)
    {
        if (!setTarget)
        {
            int curIndex = targetIndex;
            int moveTo = curIndex;
            while (curIndex != myIndex)
            {
                moveTo = curIndex;
                Grid::Node* temp = grid.getNode(curIndex);
                curIndex = temp->GetParent();
            }

            //float impulse = 1.0f;
            std::vector<float> nextPos = grid.getPosition(moveTo);
            myIndex = moveTo;

            targetX = nextPos[0];
            targetZ = nextPos[1];
            setTarget = true;
            moving = true;
            found = false;
            set = false;
            setTarget = false;
            while (!PQ.empty())
            {
                PQ.pop();
            }
            grid.ResetNodes();
            return;

        }



        /*
        else
        {
          float dispX = targetX - posX;
          float dispZ = targetZ - posZ;
          float speed = 0.07f;
          float distance = dispX * dispX + dispZ * dispZ;
          if (distance <= 0.01f)
          {
            found = false;
            set = false;
            setTarget = false;
            // empty PQ
            while (!PQ.empty())
            {
              PQ.pop();
            }
            grid.ResetNodes();
            return;
          }
          float root = sqrt(distance);
          float constant = speed / root;
          float tX = dispX * constant;
          float tZ = dispZ * constant;
          //moveX = tX;
          //moveZ = tZ;
          physics->Teleport(tX, 0, tZ);
        }
        */
    }

    if (moving)
    {
        astarMove(targetX, targetZ);
    }
}

void AstarBehavior::astarMove(float tx, float tz)
{
    GameObject* astar = ObjectManager::GetObjectByID(GetParent());

    Physics* physics = astar->GetComponent<Physics>();
    Transform* transform = astar->GetComponent<Transform>();
    float posX = transform->GetPositionX();
    float posZ = transform->GetPositionZ();

    float dispX = targetX - posX;
    float dispZ = targetZ - posZ;
    float speed = 0.07f;
    float distance = dispX * dispX + dispZ * dispZ;
    if (distance <= 0.01f)
    {
        moving = false;

        return;
    }
    float root = sqrt(distance);
    float constant = speed / root;
    float tX = dispX * constant;
    float tZ = dispZ * constant;
    //moveX = tX;
    //moveZ = tZ;
    physics->Teleport(tX, 0, tZ);
}


