/******************************************************************************
  file: BehaviorManager.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#include <array>
#include "ComponentManager.h"
#include "Behavior.h"

class Behavior;

typedef enum BehaviorType {
  BT_INVALID = -1,
 
  BT_PLAYER,
  BT_AI,
  BT_BULLET,
  BT_FLAG,
  BT_GAUGE,
  BT_ASTAR,
  BT_POWERUP,
  BT_FUELDEPOT,

  BT_NUM
} BehaviorType;

/*  BehaviorManager Works a little differently than other Component Managers  *
 *  because there are different behavior types that may have different sizes  *
 *  each behavior components memory has to be allocated when they are created *
 *  instead of allocating them all up front.                                  */
class BehaviorManager : public ComponentManager {
public:
  static BehaviorManager* GetInstance();
  ~BehaviorManager();

  void Init() override;
  void Update(float dt) override;
  void Draw() override;
  void Shutdown() override;


  /*  reallocate the behavior for the object needed because the Allocate   *
   *  function called in the objectmanager does not handl different sized  *
   *  components of the same type                                          */
  static Behavior* ReallocateBehaviorType(int behavIdx, BehaviorType behaviorType, int parentID);

private:
  BehaviorManager();
  static BehaviorManager* instance_;
  static std::array<BehaviorType, OM_MAX_OBJECTS> behaviorTypes_;
  static std::array<Component*, OM_MAX_OBJECTS>* behavComponents_;
  static std::array<bool, OM_MAX_OBJECTS>* allocatedBehaviors_;
};
