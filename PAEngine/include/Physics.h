/******************************************************************************
  * Author : Cody Morgan
  * Class  : GAM 300
  * Brief  : Integration of Bullet Physics

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once
#ifndef PHYSICS_H
#define PHYSICS_H

#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletDynamics/ConstraintSolver/btHingeConstraint.h"

#include <stdio.h>
#include <vector>
#include <algorithm>
#include "Component.h"

typedef btVector3 btVector3;
typedef void(*OnCollision(unsigned myID, unsigned otherID, const std::string& other));


/******************************************************************************
   Brief : create a box collider that is managed through bullet

   Paras : ID  - component ID
     Scale - x,y,z defualts to 1
     mass  - defualts to 0 which is STATIC and will NOT move
     pos   - x,y,z defaults to origin

  Usage: call UpdatePhysics - these objects do not update themselves - dont
     try to update them yourself

******************************************************************************/
class BoxCollider
{
public:
    /////***** Construction / Destruction *****/////

    // create a box from the engine
    BoxCollider(int ID, float xScale = 1.0f, float yScale = 1.0f, float zScale = 1.0f, float mass = 0.0f,
        float xPos = 0.0f, float yPos = 0.0f, float zPos = 0.0f);

    // destructor
    ~BoxCollider();


    /////***** Getters / Setters *****/////


    // get the ID you gave to this object
    int getID();

    // get the transform matrix in column major form - which is appearently a good thing
    std::vector<float>& getTransform();

    // get velocity as a pointer for sound
    float* const getVelocity();
    // getLinearVelocity
    inline const btVector3 getLinearVelocity() const { return rigidbody->getLinearVelocity(); }

    // get velocity vector relative to a given point in space def
    btVector3 getVelocityVector(float x = 0, float y = 0, float z = 0);

    void setFriction(float friction);

    void setMass(float mass);

    btVector3 getScale();

    void setScale(float x, float y, float z);

    int getWallID();

    void setWallID(int id);

    btVector3 getPosition();
    void setPosition(btVector3 position);

    /////***** Misc Functionality *****/////


    // destroy this box
    void destroy();

    void explode(float radius, float force);

    // teleport the object to this location
    void translate(float x, float y, float z);
    void translate(btVector3 position);

    /******************************************************************************
       Brief : determines if two colliders are colliding
       Input : with - use this if you are interested only in a specific object

    ******************************************************************************/
    void addCollisionCallback(OnCollision* onCollisionFunc, std::string target = "_ANYTHING_");
    void ignoreCollisionWithObject();
    std::vector<std::pair<std::string, OnCollision*>>& getCollisionTargets();
    void clearCollisionTargets();
    void callOnCollision(int myID, int otherID, std::string& other);

    // instead of teleporting - give the object a little push
    void addImpulse(btVector3 direction, btVector3 origin = btVector3(0, 0, 0));

    void setAngularVelocity(const btVector3 ang_vel);
    void setAngularMotor(const float vel, const float imp, int index = 0);

    void findObjectsWithinRadius(float radius, btAlignedObjectArray<btCollisionObject*>& collisionsOut);
    /////***** Public Data *****/////


    // rigidbody data only provided to you if you want to get freaky with the BTphysics
    btRigidBody* rigidbody;

private:
    int componentID_ = -1;
    float velocity_ = -1;
    btVector3 scale_;
    float mass_;
    int wallID_ = -1;
    std::vector<float> transform_;

    // use this callback if you are looking for a unique name
    std::vector<std::pair<std::string, OnCollision*>> collisionTargets_;
};

class Vehicle : public BoxCollider
{
public:
    Vehicle(int ID, float xScale = 1.0f, float yScale = 1.0f, float zScale = 1.0f, float mass = 0.0f,
        float xPos = 0.0f, float yPos = 0.0f, float zPos = 0.0f);

private:
};

///////////////////////////////////////////////////////////////////////////////
// btHingeConstraint //////////////////////////////////////////////////////////

// btHingeAccumulatedAngleConstraint
btHingeAccumulatedAngleConstraint* setHinge(btRigidBody& rbA, btRigidBody& rbB,
    const btVector3& pivotInA, const btVector3& pivotInB,
    const btVector3& axisInA, const btVector3& axisInB);

// converts the given radian range from 0 to M_PI_DOUBLE (360)
inline float convert2PositiveRadian(const float radian)
{
    if (radian < 0)
        return radian + M_PI_DOUBLE;
    else if (radian > M_PI_DOUBLE)
        return radian - M_PI_DOUBLE;

    return radian;
};

// converts the given radian range from -M_PI_2 to M_PI_2 (-90 ~ +90)
float convert2PI2RangeRadian(const float radian);

float convertPositiveRadian2PIRangeRadian(const float radian);

bool isOutOfPIRange(const float radian);


/******************************************************************************
  Brief : Public calls to init, update and shutdown the physical world

  Usage : init once, update once pre frame, shutdown once

******************************************************************************/
void InitPhysics();
void UpdatePhysics(double dt);
void ShutdownPhysics();
#endif
