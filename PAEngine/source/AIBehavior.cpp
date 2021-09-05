/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#include "pch.h"
#include "AIBehavior.h"
#include "Transform.h" 
#include "ObjectManager.h"
//#include "Grid.h"
#include "ChildTransform.h"
#include "Transform.h"
#include "GameObject.h"
#include "EnvironmentManager.h"
#include "EventSystem.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "Physics.h"
#include "ObjectFactory.h"
#include "BulletBehavior.h"
#include "PlayerBehavior.h"
#include "ModelManager.h"
#include "ModelComponent.h"

#include "AIBehavStateDefault.h"

#define AI_BARREL_ROTATION_SPEED 2.f

void BulletCollidesWithAI_CB(unsigned myID, unsigned otherID, const std::string& target)
{
  AIBehavior* AIbehav = ObjectManager::GetObjectByID(myID)->GetComponent<AIBehavior>();
  BulletBehavior* bulletbehav = ObjectManager::GetObjectByID(otherID)->GetComponent<BulletBehavior>();
  if (bulletbehav->GetShooterID() != myID)
  {
    if (AIbehav)
    {
      PlayerBehavior* otherbehav = ObjectManager::GetObjectByID(bulletbehav->GetShooterID())->GetComponent<PlayerBehavior>();
      if (otherbehav)
      {
        AIbehav->DecrementHP();
        if (AIbehav->GetHP() < 0)
        {
          AIbehav->ResetHP();
          AIbehav->SetOwner(bulletbehav->GetShooterID());
          AIbehav->SetColor(bulletbehav->GetShooterID());
          EventSystem::SendEvent(ET_SOUND_OS_TURRETCAPTURE, nullptr, 0);
        }
      }
    }
  }
}


AIBehavior::AIBehavior()
  : gamepad_(nullptr), forces_(nullptr) {
  AddBehaviorState(new AIBehavStateDefault(this));

  currentAimRadianX = 0.f;
  currentAimRadianY = 0.f;

  currentAimUnitVecX_X = sin(currentAimRadianX);
  currentAimUnitVecX_Z = cos(currentAimRadianX);
  currentAimUnitVecY_Y = sin(currentAimRadianY);
  currentAimUnitVecY_Z = cos(currentAimRadianY);
}

AIBehavior::~AIBehavior() {
}

void AIBehavior::Create(int parentID) {
  ChangeState(PBS_PLAYING);
  ObjectManager::GetInstance()->GetObjectByID(this->GetParentID())->GetComponent<Physics>()->box->addCollisionCallback((OnCollision*)&BulletCollidesWithAI_CB, "bullet");

}

void AIBehavior::Destroy() {
  gamepad_ = nullptr;
  Behavior::Shutdown();
}

void AIBehavior::DefaultUpdate()
{
	//  update AIs movement from gamepad
  GameObject* thisObject = ObjectManager::GetObjectByID(GetParentID());

  HeadRotate();

  if (owner == -1)
  {
    return;
  }


  

  //GameObject* player = ObjectManager::GetObjectsByName("player")[0];
  std::vector<GameObject*> players = ObjectManager::GetObjectsByName("player");

  Physics* physics = thisObject->GetComponent<Physics>();
  Transform* transform = thisObject->GetComponent<Transform>();
   
  float posX = transform->GetPositionX();
  float posZ = transform->GetPositionZ();


  if (target == -1)
  {
    //int index = -1;
    for (std::vector<GameObject*>::iterator it = players.begin(); it != players.end(); ++it)
    {
      //++index;
      if ((*it)->GetComponent<PlayerBehavior>()->GetPlayerID() == owner)
      {
        continue;
      }
      Transform* playerTransform = (*it)->GetComponent<Transform>();
      float playerX = playerTransform->GetPositionX();
      float playerZ = playerTransform->GetPositionZ();

      float dispX = playerX - posX;
      float dispZ = playerZ - posZ;
      float distance = dispX * dispX + dispZ * dispZ;
      if (distance <= 0)
      {
        continue;
      }
      if (distance > range * 3)
      {
        continue;
      }
      
      target = (*it)->GetID();
      targetID = (*it)->GetComponent<PlayerBehavior>()->GetPlayerID();
      break;
    }
  }
  else
  {
    if (targetID == owner || ObjectManager::GetObjectByID(target)->GetComponent<PlayerBehavior>()->HP < 1)
    {
      target = -1;
      targetID = -1;
      return;
    }

    Transform* playerTransform = ObjectManager::GetObjectByID(target)->GetComponent<Transform>();
    float playerX = playerTransform->GetPositionX();
    float playerY = playerTransform->GetPositionY();
    float playerZ = playerTransform->GetPositionZ();

    float dispX = playerX - posX;
    float dispZ = playerZ - posZ;
    float distance = dispX * dispX + dispZ * dispZ;
    //printf("%f\n", distance);
    if (distance > 0 && distance < 3 * range)
    {
      float unitDist = sqrtf(distance);
      dispX /= unitDist;
      dispZ /= unitDist;
      if (moveXaxis)
      {
        physics->Teleport(dispX * speed, 0, 0);
      }
      else
      {
        physics->Teleport(0, 0, dispZ * speed);
      }
    }
    if (distance > range)
    {
      target = -1;
      targetID = -1;
      return;
    }

    

    SetHeadDirection(playerX, playerY, playerZ);
    float firerate = 4.0f;
    if (GetTimer() - time_ >= firerate)
    {

      EventSystem::SendEvent(ET_SOUND_OS_PLAYERSHOOT, nullptr, 0);

      Transform* trans = thisObject->GetComponent<Transform>();
      int bulletID = ObjectFactory::CreateBullet(GetParentID(), trans->GetPositionX(), trans->GetPositionY() + 2., trans->GetPositionZ(), (playerX - posX) / sqrt(distance), .2,
        (playerZ - posZ) / sqrt(distance), 1, 20);

      ObjectManager::GetObjectByID(bulletID)->GetComponent<BulletBehavior>()->SetTargetID(target);
      ObjectManager::GetObjectByID(bulletID)->GetComponent<BulletBehavior>()->ttl_ = 1.5f;
      ObjectManager::GetObjectByID(bulletID)->GetComponent<BulletBehavior>()->SetBulletParameters(false, true);
      

      time_ = GetTimer();

    }
  }

  



  //float speed = 0.1f;



  //distance = speed / sqrt(distance);
  //physics->Teleport(dispX * distance, 0, dispZ * distance);
  

  /*
//  Get my position
  Component* componentTransform = ObjectManager::GetComponent(GetParentID(), CT_TRANSFORM);
  Transform* transform = reinterpret_cast<Transform*>(componentTransform);
  float posX = transform->GetPositionX();
  float posZ = transform->GetPositionZ();
//  Get my node index
  Grid &grid = EnvironmentManager::getGrid();
  
  //Grid grid = Grid(100, 100, 1);
  int index = grid.getNodeIndex(posX, posZ);
  Grid::Node* node = grid.getNode(index);
  

//  Get target position
//  Get target node index
  int targetIndex = grid.getNodeIndex(playerX, playerZ);
  if (index == targetIndex)
  {
    return;
  }
  //  Set my node's distance to 0
  node->SetDistance(0);
  //  Create a priority queue
  std::priority_queue<Grid::Node*> PQ;
//  for each node in grid.nodes set heuristic
  grid.SetHeuristicsAll(targetIndex);
//  Add my node's neighbor to the PQ and set InQueue
  std::vector<int> neighbors = node->GetNeighbors();
  for (std::vector<int>::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
  {
    Grid::Node* neigh = grid.getNode(*it);
    neigh->SetParent(index);
    neigh->Enqueue(true);
    neigh->SetDistance(node->GetDistance() + 1);
    PQ.push(neigh);
  }
  bool found = false;
  
//  while PQ is not empty
  while (!PQ.empty())
  {
    //    get PQ.top
    Grid::Node* next = PQ.top();
    //    if top is goal found = true, break
    if (next->GetIndex() == targetIndex)
    {
      found = true;
      break;
    }
    //    remove top from PQ
    PQ.pop();
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
      if (n->GetDistance() > topDistance + 1)
      {
        //n.dist = newDist
        n->SetDistance(newDist);
        //n.parent = top
        n->SetParent(topIndex);
        //n.enqueue (if not already in queue)
        if (!n->IsInQueue())
        {
          n->Enqueue(true);
          PQ.push(n);
        }
      }
    }

  }
//
//  if found
//    backtrack from goal until my index
  if (found)
  {
    int curIndex = targetIndex;
    int moveTo = curIndex;
    while (curIndex != index)
    {
      moveTo = curIndex;
      Grid::Node* temp = grid.getNode(curIndex);
      curIndex = temp->GetParent();
    }

    //float impulse = 1.0f;
    std::vector<float> nextPos = grid.getPosition(moveTo);


    float dispX = nextPos[0] - posX;
    float dispZ = nextPos[1] - posZ;

    float speed = 0.3f;
    float distance = dispX * dispX + dispZ * dispZ;
    if (distance <= 0)
    {
      grid.ResetNodes();
      return;
    }
    float root = sqrt(distance);
    float constant = speed / root;
    float tX = dispX * constant;
    float tZ = dispZ * constant;
    moveX = tX;
    moveZ = tZ;
    //physics->Teleport(tX, 0, tZ);
  }
//  else
//    exception


  grid.ResetNodes();
  */
  
  /*
  float dispX = playerX - posX;
  float dispZ = playerZ - posZ;

  float speed = 0.3f;
  float distance = dispX * dispX + dispZ * dispZ;
  if (distance <= 0)
  {
    return;
  }
  distance = speed / sqrt(distance);
  //Component* componentPhysics = ObjectManager::GetComponent(GetParentID(), CT_PHYSICS);
  //Physics* physics = reinterpret_cast<Physics*>(componentPhysics);
  physics->Teleport(dispX* distance, 0, dispZ* distance);
  */
  //physics->Teleport(-0.3f, 0, 0);
};

void AIBehavior::SetColor(int id)
{
  GameObject* thisObject = ObjectManager::GetObjectByID(GetParentID());
  PlayerBehavior* otherbehav = ObjectManager::GetObjectByID(id)->GetComponent<PlayerBehavior>();
  for (auto childObj : thisObject->GetChildren())
  {
    ModelComponent* pModel = ObjectManager::GetObjectByID(childObj)->GetComponent<ModelComponent>();
    if (pModel != nullptr)
    {
      ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
        otherbehav->colorOrigin.x, otherbehav->colorOrigin.y, otherbehav->colorOrigin.z);
    }
  }
}

void AIBehavior::SetOwner(int id)
{
  owner = ObjectManager::GetObjectByID(id)->GetComponent<PlayerBehavior>()->GetPlayerID();
}

int AIBehavior::GetOwner()
{
  return owner;
}

void AIBehavior::DecrementHP()
{
  --HP;
}

void AIBehavior::ResetHP()
{
  HP = 5;
}

int AIBehavior::GetHP()
{
  return HP;
}

const int AIBehavior::GetRange()
{
  return range;
}

void AIBehavior::SetTarget(int id)
{
  target = id;
}

int AIBehavior::GetTarget()
{
  return target;
}

// move the direction by the given raidan (yaw, pitch)
void AIBehavior::SetHeadDirection(const float horizontalRadian, const float verticalRadian)
{
  currentAimRadianX = horizontalRadian; // target yaw
  currentAimRadianY = verticalRadian;   // target pitch

  // it keeps the radian between 0 - 360 degree
  currentAimRadianX = convert2PositiveRadian(currentAimRadianX);
  currentAimRadianY = convert2PositiveRadian(currentAimRadianY);

  // yaw
  currentAimUnitVecX_X = sin(currentAimRadianX);
  currentAimUnitVecX_Z = cos(currentAimRadianX);

  // pitch
  currentAimUnitVecY_Y = sin(currentAimRadianY);
  currentAimUnitVecY_Z = cos(currentAimRadianY);
}

// move the direction to the given target position
// parameters are the position of Transform compoment
void AIBehavior::SetHeadDirection(const float _x, const float _y, const float _z)
{
  Physics* pBodyPhys = ObjectManager::GetObjectByID(partBodyID)->GetComponent<Physics>();

  btVector3 targetPos(_x, -_y, _z);
  btVector3 nVector = (pBodyPhys->box->getPosition() - targetPos).normalized();

  currentAimUnitVecX_X = nVector.getX();
  currentAimUnitVecX_Z = nVector.getZ();

  currentAimUnitVecY_Y = nVector.getY();
  currentAimUnitVecY_Z = nVector.getZ();
}

void AIBehavior::HeadRotate()
{
  Physics* pHeadPhys = ObjectManager::GetObjectByID(partHeadID)->GetComponent<Physics>();
  Physics* pBodyPhys = ObjectManager::GetObjectByID(partBodyID)->GetComponent<Physics>();

  // Head: horizontal
  {
    btHingeAccumulatedAngleConstraint* pAccuHingeBody = reinterpret_cast<btHingeAccumulatedAngleConstraint*>(pBodyPhys->box->rigidbody->getConstraintRef(0));
    const float currentHeadRadian = pAccuHingeBody->getHingeAngle() + M_PI_2;
    const float currentHeadUnitVecX = sin(currentHeadRadian);
    const float currentHeadUnitVecY = -cos(currentHeadRadian);

    const float crossProduct = (currentAimUnitVecX_X * currentHeadUnitVecY) - (currentAimUnitVecX_Z * currentHeadUnitVecX);
    float direaction = 0;
    if (crossProduct > 0.01f)
      direaction = -1.f;
    else if (crossProduct < -0.01f)
      direaction = 1.f;

    // makes the head rotate
    pAccuHingeBody->enableAngularMotor(true, direaction * AI_BARREL_ROTATION_SPEED, 100000);
  }

  // Head: vertical
  {
    btHingeAccumulatedAngleConstraint* pAccuHingeHead = reinterpret_cast<btHingeAccumulatedAngleConstraint*>(pHeadPhys->box->rigidbody->getConstraintRef(0));
    const float currentHeadRadian = pAccuHingeHead->getHingeAngle() + M_PI_2;
    const float currentHeadUnitVecX = sin(currentHeadRadian);
    const float currentHeadUnitVecY = -cos(currentHeadRadian);

    const float crossProduct = (currentAimUnitVecY_Z * currentHeadUnitVecY) - (currentAimUnitVecY_Y * currentHeadUnitVecX);
    float direaction = 0;
    if (crossProduct > 0.01f)
      direaction = -1.f;
    else if (crossProduct < -0.01f)
      direaction = 1.f;

    // makes the head rotate
    pAccuHingeHead->enableAngularMotor(true, direaction * AI_BARREL_ROTATION_SPEED, 100000);
  }
}

//Astar
//{
//  Get my position
//  Get my node index
//  Set my node's distance to 0
//  Create a priority queue
//  Get target position
//  Get target node index
//  for each node in grid.nodes set heuristic
//  Add my node's neighbor to the PQ and set InQueue
//  while PQ is not empty
//    get PQ.top
//    if top is goal found = true, break
//    remove top from PQ
//    Get the neighbors of top
//    for each neighbor n
//      if not wall
//        newDist = top.dist + 1
//        if newDist < n.dist
//          n.dist = newDist
//          n.parent = top
//          n.enqueue (if not already in queue)
//
//  if found
//    backtrack from goal until my index
//  else
//    exception
//}
