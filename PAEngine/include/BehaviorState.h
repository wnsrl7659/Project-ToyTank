/******************************************************************************
  file: BahaviorState.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#include "Behavior.h"

/*****************************************************************************
  base class for behavior states
******************************************************************************/
class BehaviorState {
public:
  BehaviorState() {};
  ~BehaviorState() {};

  /*  Init is called when the state is entered  */
  virtual void Init() = 0;
  /*  Update is called every frame the state is active on the parent behavior  */
  virtual void Update(float dt) = 0;
  /*  Shutdown is called before the parent behavior changes to a new state*/
  virtual void Shutdown() = 0;
};

