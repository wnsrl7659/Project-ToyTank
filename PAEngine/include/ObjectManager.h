/******************************************************************************
  filename: ObjectManager.h
  author: j.mcdonell  

  (c) DigiPen Institute of Technology 2019
******************************************************************************/
#pragma once
#include <array>
#include <vector>
#include <string>
#include <unordered_set>
#include "GameObject.h"
#include "ComponentManager.h"


class Component;
class ComponentManager;
class GameObject;

/*  This class is used to build gameobjects with a set of components     *
 *  The gameObjects and their components can then be accessed using the  *
 *  ID returned from the 'CreateObject' function                         */
class ObjectManager : public ComponentManager {
public:
  /*  Creates a single instance of this class and  *
   *  returns a pointer to it                      */
  static ObjectManager* GetInstance();
  ~ObjectManager();

  /*  Destroys an object with the given ID  */
  static void DestroyObject(int objectID, int delay = 0);

  /*  We can ask the ObjectManager for an object with a unique ID, or  *
   *  we can get a vector of objects with a common name                */
  static GameObject* GetObjectByID(int objectID);
  static std::vector<GameObject*> GetObjectsByName(const std::string& name);

  static std::unordered_set<int> GetChildObjects(int objectID);

  /*  Sets a parent-child relationship between two objects  */
  static void MakeParent(int childID, int parentID);

  /*  removes the parent-child relationship for this object  */
  static void RemoveParent(int childID);

  /*  Destroys all the objects components and Deallocates them in the IndexManager  */
  static void ClearObjects();

  /*  checks if the give object ID is valid  */
  static bool IsValidID(int objectID);

  
  /*  initializes component managers, clears data about objects and their ID's  */
  void Init();
  /*  udpates each components manager                                           */
  void Update(float dt);
  /*  calls the draw function on each component manager                         */
  void Draw();
  /*  Shuts down component managers, clears data about objects and their ID's   */
  void Shutdown();

private:
  static ObjectManager* instance_;
  std::vector<ComponentManager*> componentManagers_;

  /*  allocates memory needed for the component managers  */
  ObjectManager(); 
};
