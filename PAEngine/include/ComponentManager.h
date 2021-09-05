/******************************************************************************
  file: ComponentManager.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#define OM_MAX_OBJECTS 4096

class Component;
/*  Base class for other component managers (TransformManager, BehaviorManager, etc.)  *
 *  this class keeps track of the active components and keeps them updated             */
class ComponentManager {
public:
  ComponentManager();
  virtual ~ComponentManager();
  /*  finds an open spot for a component to be initialized           *
   *  return the components index or -1 if ComponentManager is full  */
  int Allocate();
  /*  marks the index as free so it can be reallocated  */
  void Deallocate(int componentIndex);

  /*  gets the address of the component at the specified index, use this when  *
   *  you want to access the componenet associated with an object ID           */
  Component* Get(int componentIndex);

  virtual void Init() = 0;
  virtual void Update(float dt) = 0;
  virtual void Draw() = 0;
  virtual void Shutdown() = 0;

protected:
  std::array<Component*, OM_MAX_OBJECTS> components_;
  std::array<bool, OM_MAX_OBJECTS> allocated_;
private:
};



