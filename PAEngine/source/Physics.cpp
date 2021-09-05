/******************************************************************************
  * Author : Cody Morgan
  * Class  : GAM 300
  * Brief  : Integration of Bullet Physics

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma warning(disable : 4099) // debug data no available for bt
#pragma warning(disable : 26495)// bt warnings
#pragma warning(disable : 26451)

#include "pch.h"
using std::string;
using std::vector;

#include <stdio.h>
#include <iostream>
using std::cout;
#include <map>

#include "BulletCollision/NarrowPhaseCollision/btPersistentManifold.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "Physics.h"
#include "ObjectManager.h"
#include "GameObject.h"
#include "serviceLocator.h" // log
#include "Transform.h"

#define _USE_MATH_DEFINES
#include <math.h>

static std::map<std::string, bool> objectIgnoreList; // [name]: what to ignore use _ANYTHING_ to ignore all collisions(default)

void CollisionCallback(btPersistentManifold* const& manifold)
{
  // get the collider objects out of the manifold
  const btCollisionObject* objectA = manifold->getBody0();
  const btCollisionObject* objectB = manifold->getBody1();
  GameObject* objA = ObjectManager::GetObjectByID(objectA->getUserIndex());
  GameObject* objB = ObjectManager::GetObjectByID(objectB->getUserIndex());
  string nameA;
  string nameB;

  // get name from id
  if (objA && objB)
  {
    nameA = objA->GetName();
    nameB = objB->GetName();
  }
  else
    return;

  // this object may be colliding - but it may need to be ignored
  if (objectIgnoreList.find(nameA) != objectIgnoreList.end() || objectIgnoreList.find(nameB) != objectIgnoreList.end())
  {
    return;
  }


  // get component 
  Physics* physicsA = ObjectManager::GetObjectByID(objectA->getUserIndex())->GetComponent<Physics>();
  Physics* physicsB = ObjectManager::GetObjectByID(objectB->getUserIndex())->GetComponent<Physics>();

  if (!physicsA || !physicsB)
    return;

  // get the box
  BoxCollider* boxA = physicsA->box;
  BoxCollider* boxB = physicsB->box;

  if (!boxA || !boxB)
    return;

  // get targets aTarget is a's Target
  std::vector<std::pair<std::string, OnCollision*>>& aTargets = boxA->getCollisionTargets();
  std::vector<std::pair<std::string, OnCollision*>>& bTargets = boxB->getCollisionTargets();

  if (!aTargets.empty())
  {
    for (auto aPair : aTargets)
    {
      // 2 non empty objects are cross checked
      if (bTargets.size() > 0)
      {
        for (auto bPair : bTargets)
        {
          string aTarget = aPair.first;
          string bTarget = bPair.first;

          // a's target is b or a triggers on anyhting
          if (!aTarget.empty() && (nameB.find(aTarget) != string::npos || aTarget == "_ANYTHING_"))
          {
            int objA_id = objectA->getUserIndex();
            int objB_id = objectB->getUserIndex();

            GameObject* objA = ObjectManager::GetObjectByID(objA_id);
            GameObject* objB = ObjectManager::GetObjectByID(objB_id);

            if (objA && objB)
              boxA->callOnCollision(objA_id, objB_id, aTarget);
          }

          if (!bTarget.empty() && (nameA.find(bTarget) != string::npos || bTarget == "_ANYTHING_"))
          {
            int objA_id = objectA->getUserIndex();
            int objB_id = objectB->getUserIndex();

            GameObject* objA = ObjectManager::GetObjectByID(objA_id);
            GameObject* objB = ObjectManager::GetObjectByID(objB_id);

            if (objA && objB)
              boxB->callOnCollision(objB_id, objA_id, bTarget);
          }
        }
      }

      // empty objects still need to be checked against all A's targets
      else
      {
        string aTarget = aPair.first;
        int objA_id = objectA->getUserIndex();
        int objB_id = objectB->getUserIndex();

        GameObject* objA = ObjectManager::GetObjectByID(objA_id);
        GameObject* objB = ObjectManager::GetObjectByID(objB_id);

        if (!objA || !objB)
          break;

        // a's target is b or a triggers on anyhting
        if (!aTarget.empty() && (nameB.find(aTarget) != string::npos || aTarget == "_ANYTHING_"))
        {
          boxA->callOnCollision(objectA->getUserIndex(), objectB->getUserIndex(), aTarget);
        }
      }
    }
  }

  // dumb way of doing this don't jundge me joey
  else
  {
    for (auto bPair : bTargets)
    {
      string bTarget = bPair.first;

      // a's target is b or a triggers on anyhting
      if (!bTarget.empty() && (nameA.find(bTarget) != string::npos || bTarget == "_ANYTHING_"))
      {
        boxB->callOnCollision(objectB->getUserIndex(), objectA->getUserIndex(), bTarget);
      }
    }
  }
}

class BulletMgr
{
public:
  BulletMgr();
  ~BulletMgr();

  btDiscreteDynamicsWorld* getworld()
  {
    return dynamicsWorld;
  };

  btDefaultCollisionConfiguration* collisionConfiguration; // collision memory and setup. modification is for adv users
  btCollisionDispatcher* dispatcher;     // single thread dispatcher, mt: check out extras/bulletmultithread
  btBroadphaseInterface* overlappingPairCache;   // general purpose checkout btAxis3Sweep   
  btSequentialImpulseConstraintSolver* solver;     // constraint solver. mt: check out extras/bulletmultithread
  btDiscreteDynamicsWorld* dynamicsWorld;    // pointer to actual simulation
  btAlignedObjectArray<btCollisionShape*> collisionShapes;  // array of colliders

};
static BulletMgr* bulletManager = nullptr;


/////***** Box Collider *****/////



BoxCollider::BoxCollider(int ID, float x, float y, float z, float mass, float xPos, float yPos, float zPos) :
  mass_(mass), scale_(btVector3(x, y, z)), componentID_(ID)
{
  // create box geometry
  btCollisionShape* colliderShape = new btBoxShape(btVector3(x, y, z));
  colliderShape->setMargin(0.1);
  bulletManager->collisionShapes.push_back(colliderShape);

  // setup transform
  btTransform trans;
  trans.setIdentity();
  trans.setOrigin(btVector3(xPos, yPos, zPos));

  //rigidbody is dynamic if and only if mass is non zero, otherwise static
  bool isDynamic = (mass != 0.f);

  btVector3 localInertia(0, 0, 0);
  if (isDynamic)
    colliderShape->calculateLocalInertia(mass, localInertia);

  //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
  btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colliderShape, localInertia);
  rigidbody = new btRigidBody(rbInfo);
  rigidbody->setActivationState(true);
  rigidbody->setFriction(1.0f);

  // save the id in bullet for collision detection
  rigidbody->setUserIndex(ID);
  rigidbody->setUserIndex2(ID);
  rigidbody->setUserIndex3(ID);
  ID = rigidbody->getUserIndex();
  cout << ID << " : " << ObjectManager::GetObjectByID(ID)->GetName() << "\n";

  //rigidbody->setCollisionFlags(rigidbody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

  bulletManager->getworld()->addRigidBody(rigidbody);

  transform_ = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
}

BoxCollider::~BoxCollider()
{
  if (rigidbody)
  {
    // release all constraints which is linked with the rigidbody
    while (rigidbody->getNumConstraintRefs())
    {
      btTypedConstraint* constraint = rigidbody->getConstraintRef(0);
      bulletManager->getworld()->removeConstraint(constraint);
      delete constraint;
    }

    delete rigidbody->getMotionState();
    delete rigidbody->getCollisionShape();
    bulletManager->getworld()->removeRigidBody(rigidbody);

    delete rigidbody;
    rigidbody = nullptr;
  }
}

void BoxCollider::destroy()
{
  this->~BoxCollider();
}

void BoxCollider::findObjectsWithinRadius(float radius, btAlignedObjectArray<btCollisionObject*>& collisionsOut)
{
  btGhostPairCallback* ghostCall = new btGhostPairCallback();
  bulletManager->getworld()->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(ghostCall);

  btGhostObject* ghostObj = new btGhostObject();
  btCollisionShape* shape = new btSphereShape(radius);
  ghostObj->setCollisionShape(shape);
  btTransform trans;
  trans.setIdentity();
  trans.setOrigin(rigidbody->getWorldTransform().getOrigin());
  ghostObj->setWorldTransform(trans);
  ghostObj->setCollisionFlags(ghostObj->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
  bulletManager->dynamicsWorld->addCollisionObject(ghostObj, btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger);

  int io = bulletManager->dynamicsWorld->getCollisionObjectArray().size();
  collisionsOut.resize(0);
  for (size_t i = 0; i < bulletManager->dynamicsWorld->getCollisionObjectArray().size(); i++)
  {
    btCollisionObject* obj = bulletManager->dynamicsWorld->getCollisionObjectArray()[i];
    btRigidBody* body = btRigidBody::upcast(obj);

    btAlignedObjectArray <btCollisionObject*>* pObjsInsideGhostObject = nullptr;
    btGhostObject* ghost = btGhostObject::upcast(obj);

    if (ghost) 
    {
      collisionsOut = ghost->getOverlappingPairs();
      //for (size_t j = 0; j < collisionsOut.size(); j++)
      //{
      //  btRigidBody* rb = btRigidBody::upcast(collisionsOut[j]);
      //  cout << "object : " << rb[j].getUserIndex();
      //  GameObject* go = ObjectManager::GetObjectByID(rb[j].getUserIndex());
      //  if (go)
      //  {
      //    cout << " name: " << go->GetName() << "\n";
      //  }
      //}
    }
  }

  bulletManager->getworld()->removeCollisionObject(ghostObj);
}

void BoxCollider::explode(float radius, float force)
{
  // gather all objects within distance
  btAlignedObjectArray<btCollisionObject*> collisionsOut;
  findObjectsWithinRadius(radius, collisionsOut);

  // apply force to each
  for (size_t i = 0; i < collisionsOut.size(); i++)
  {
    // get normalized vector from me to you
    btRigidBody* collisionObj = btRigidBody::upcast(collisionsOut[i]);
    btVector3 you = collisionObj->getWorldTransform().getOrigin();
    btVector3 me =  rigidbody->getWorldTransform().getOrigin();
    btVector3 dir =  you - me;
    if (!dir.fuzzyZero())
    {

      dir = dir.normalize();
      dir *= force;

      // apply force in that direction
      collisionObj->applyCentralImpulse(dir);
    }
  }
}

int BoxCollider::getID()
{
  return componentID_;
}

std::vector<float>& BoxCollider::getTransform()
{
  // create a matrix to modify the scale of an object along arbitrary rotation
  btMatrix3x3 modifiedScale;
  modifiedScale.setIdentity();

  // get rotation
  btQuaternion quaternion = rigidbody->getWorldTransform().getRotation();
  modifiedScale.setRotation(quaternion);

  // scale
  modifiedScale = modifiedScale.scaled(scale_);

  // apply scale along rotation axis
  rigidbody->getWorldTransform().getOpenGLMatrix(&transform_[0]);
  modifiedScale.getOpenGLSubMatrix(&transform_[0]);

  return transform_;
}

// teleport the object to this location
void BoxCollider::translate(float x, float y, float z)
{
  translate(btVector3(x, y, z));
}
void BoxCollider::translate(btVector3 trans)
{
  rigidbody->translate(trans);
}

void BoxCollider::addCollisionCallback(OnCollision* onCollisionFunc, std::string with)
{
  collisionTargets_.push_back(std::make_pair(with, onCollisionFunc));
  gContactStartedCallback = (ContactStartedCallback)CollisionCallback;
}

void BoxCollider::ignoreCollisionWithObject()
{
  objectIgnoreList[ObjectManager::GetInstance()->GetObjectByID(this->getID())->GetName()] = true;
  rigidbody->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

}

// instead of teleporting - give the object a little push
void BoxCollider::addImpulse(btVector3 direction, btVector3 origin)
{
  rigidbody->forceActivationState(DISABLE_DEACTIVATION);
  rigidbody->applyImpulse(direction, origin);
}

void BoxCollider::setAngularVelocity(const btVector3 ang_vel)
{
  rigidbody->setAngularVelocity(ang_vel);
}

void BoxCollider::setAngularMotor(const float vel, const float imp, int index)
{
  if (!rigidbody->getNumConstraintRefs())
  {
    Util::ServiceLocator::getLogger()->print(
      Util::SeverityType::error,
      "setHingeAngle: This rigidbody has no constraint");

    return;
  }

  btHingeAccumulatedAngleConstraint* pHinge = reinterpret_cast<btHingeAccumulatedAngleConstraint*>(rigidbody->getConstraintRef(index));
  pHinge->enableAngularMotor(true, vel, imp);
}

std::vector<std::pair<std::string, OnCollision*>>& BoxCollider::getCollisionTargets()
{
  return collisionTargets_;
}

void BoxCollider::clearCollisionTargets()
{
  collisionTargets_.clear();
}

void BoxCollider::callOnCollision(int myID, int otherID, std::string& other)
{
  if (!collisionTargets_.empty())
  {
    for (auto pair : collisionTargets_)
    {
      if(pair.first == other || other == "_ANYTHING_")
        (*pair.second)(myID, otherID, other);
    }
  }
}

// get velocity as a pointer for sound 
float* const BoxCollider::getVelocity()
{
  getVelocityVector();

  return &velocity_;
}

// get velocity vector relative to a given point in space def
btVector3 BoxCollider::getVelocityVector(float x, float y, float z)
{
  btVector3 vel = rigidbody->getVelocityInLocalPoint(btVector3(0.0f, 100.0f, 0.0f));
  velocity_ = vel.length();

  return vel;
}

void BoxCollider::setFriction(float friction)
{
  float oldFriction = rigidbody->getFriction();
  rigidbody->setFriction(friction);
}

void BoxCollider::setMass(float mass)
{
  //bulletManager->dynamicsWorld->removeRigidBody(rigidbody);
  btVector3 inertia;

  rigidbody->getCollisionShape()->calculateLocalInertia(mass, inertia);
  rigidbody->setMassProps(mass, inertia);
  rigidbody->updateInertiaTensor();
  rigidbody->forceActivationState(DISABLE_DEACTIVATION);

  mass_ = mass;
}

btVector3 BoxCollider::getScale()
{
  return scale_;
}

void BoxCollider::setScale(float x, float y, float z)
{
  scale_ = btVector3(x, y, z);
}


int BoxCollider::getWallID()
{
  return wallID_;
}

void BoxCollider::setWallID(int id)
{
  wallID_ = id;
}

btVector3 BoxCollider::getPosition()
{
  return rigidbody->getWorldTransform().getOrigin();
}

void BoxCollider::setPosition(btVector3 position)
{
  btTransform trans = rigidbody->getWorldTransform();
  trans.setOrigin(position);
  rigidbody->setWorldTransform(trans);
}


/////***** Vehicle Collider *****/////

Vehicle::Vehicle(int ID, float x, float y, float z, float mass, float xPos, float yPos, float zPos) : BoxCollider(ID, x, y, z, mass, xPos, yPos, zPos)
{

  //btRaycastVehicle::btVehicleTuning m_tuning;
  //btVehicleRaycaster* vehicleRayCaster;
  //btRaycastVehicle* vehicle;
  //btCollisionShape* wheelShape;

  // WIP
   // rigidbody->setActivationState(DISABLE_DEACTIVATION);
   // vehicleRayCaster = new btDefaultVehicleRaycaster(bulletManager->getworld());
   // vehicle = new btRaycastVehicle(m_tuning, rigidbody, vehicleRayCaster);

   //// bulletManager->getworld()->addVehicle(vehicle);
   // bulletManager->getworld()->getCollisionObjectArray().push_back(rigidbody);
}


/////***** Bullet Management *****/////


BulletMgr::BulletMgr()
{
  // init world
  collisionConfiguration = new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collisionConfiguration);
  overlappingPairCache = new btDbvtBroadphase();
  solver = new btSequentialImpulseConstraintSolver();
  dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

  // setup defualt values
  dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
}

BulletMgr::~BulletMgr()
{
  // delete in reverse order of creation

  // delete all rigidbodies
  for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
  {
    btCollisionObject* object = dynamicsWorld->getCollisionObjectArray()[i];
    btRigidBody* body = btRigidBody::upcast(object);
    btTransform trans;
    if (body && body->getMotionState())
    {
      delete body->getMotionState();
    }
    dynamicsWorld->removeCollisionObject(object);
    delete object;
  }

  // delete all collision objects
  for (int i = 0; i < collisionShapes.size(); i++)
  {
    btCollisionShape* shape = collisionShapes[i];
    collisionShapes[i] = nullptr;
    delete shape;
  }

  delete dynamicsWorld;
  delete solver;
  delete overlappingPairCache;
  delete dispatcher;
  delete collisionConfiguration;
}

///////////////////////////////////////////////////////////////////////////////
// btHingeConstraint //////////////////////////////////////////////////////////

// usually the object A is the root.
// the default values for hinge is static. (no spring effect and no spinning)
btHingeAccumulatedAngleConstraint* setHinge(btRigidBody& rbA, btRigidBody& rbB,
  const btVector3& pivotInA, const btVector3& pivotInB,
  const btVector3& axisInA, const btVector3& axisInB)
{

  btHingeAccumulatedAngleConstraint* pHinge = new btHingeAccumulatedAngleConstraint(
    rbA, rbB,
    pivotInA, pivotInB,
    axisInA, axisInB);

  // add constraint to world
  bulletManager->getworld()->addConstraint(pHinge, true);

  return pHinge;
}

float convert2PI2RangeRadian(const float radian)
{
  if (radian > M_PI_2)
    return M_PI_2 - (radian - M_PI_2);
  else if (radian < -M_PI_2)
    return -M_PI_2 - (radian + M_PI_2);

  return radian;
};

float convertPositiveRadian2PIRangeRadian(const float radian)
{
  if (radian > M_PI)
    return (radian - M_PI) - M_PI;
  else if (radian < -M_PI)
    return (radian + M_PI) + M_PI;


  return radian;
}

bool isOutOfPIRange(const float radian)
{
  if (radian > M_PI_2)
    return true;
  else if (radian < -M_PI_2)
    return true;

  return false;
}

/////***** Public Management *****/////

void InitPhysics()
{
  bulletManager = new BulletMgr();


  //BoxCollider* ground = bulletManager->createBox(100, 1, 100, 0, 0, -1, 0);
  //BoxCollider* box = bulletManager->createBox(10, 10, 10, 1, 0, 500, 0);
  //box->rigidbody->applyImpulse(btVector3(0, 10, 0), btVector3(-5, 5, 0));
}

// Testing purposes
void PrintObjectLocations()
{

#ifdef _DEBUG
  for (int i = bulletManager->getworld()->getNumCollisionObjects() - 1; i >= 0; i--)
  {

    btCollisionObject* obj = bulletManager->getworld()->getCollisionObjectArray()[i];
    btRigidBody* rb = btRigidBody::upcast(obj);


    float test[16];
    rb->getWorldTransform().getOpenGLMatrix(test);

    btTransform trans;
    if (rb && rb->getMotionState())
    {
      rb->getMotionState()->getWorldTransform(trans);
    }
    else
    {
      trans = obj->getWorldTransform();
    }
    btVector3 location(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
    if (i)
      printf("\t\t\t\t\t\t\t\t");
    printf("world pos: of obj %d = (%f, %f, %f)\n", i, location.x(), location.y(), location.z());

  }
#endif
}


void UpdatePhysics(double dt)
{
  bulletManager->getworld()->stepSimulation(dt);
}

void ShutdownPhysics()
{
  delete bulletManager;
  bulletManager = nullptr;
}

