/******************************************************************************
  file: Component.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once

class ComponentManager;
/*  This is the base class that other componets build off of. These      *
 *  components will be associated with a game object using the parentID  *
 *  as the objects id to find other components from the ObjectManager    */
class Component {
public:
  /*  Components are created with invaild parent ID's  */
  Component(int managerIdx = -1, ComponentManager* manager = nullptr);
  virtual ~Component();

  /* Sets the parent of this component, the parent can be another component */
  void SetParent(int parentID);

  /* Gets the ID of the parent component */
  const int& GetParent() const;

  /* gets the manager responsible for this component, used when we do the *
   * opertation on different component mangers                            */
  ComponentManager* GetManager() const;

  /* gets the index into the manager where this component is stored */
  const int& GetID() const;

  /* implement in derived componenents. this function should reset the data *
   * storeed in the component so the memory can be reused                   */
  virtual void Destroy() {};
protected:
  int parentID_; // id of the parent component
  int compID; // id of this component, maps to the index into the manager where it is stored
  ComponentManager* manager_; // convenient way to get the manager responsible for this component
};