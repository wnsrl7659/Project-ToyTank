/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#ifndef ENVIRON_H
#define ENVIRON_H
#pragma once
 
#include "CameraManager.h"
#include "Grid.h"

#include <queue>

typedef struct WallInfo 
{
  bool isValid = false;
  int bigwall_id = 0;    
  unsigned width = 0;         
  unsigned height = 0;
  unsigned wallNum = 0;
  vec3 position;
  float angle = 0;
  float mass = 0;
  bool isDestructable = true;
  std::vector<int> blocks;
  float startTTL = 4.0f;
  float TTL = startTTL;
  bool building = false;
};

class EnvironmentManager
{
public:
  static EnvironmentManager* getEnvironment();
  static Grid& getGrid();

  void init();
  void update(float dt);
  void shutdown();
  void restart();

  /****************************************************************************
  * brief : generate a wall in the world
  * input : width / height - dimensions of the wall in bricks
  *         bricksize      - dimensions of the bricks
  *         position       - location of the first brick
  *         angle          - angle of wall in degrees
  *         mass           - mass of brick
  *
  ****************************************************************************/
  vec3 generateWall(unsigned width, unsigned height, vec3 position, float angle, bool isBuilding, bool isDestructable = true, float mass = 0.5f);

  vec3 generateRandomWall(unsigned width = 0, unsigned height = 0, float mass = 0.5f);

  void generateRandomBuilding(unsigned width = 0, unsigned length = 0, unsigned height = 0);

  void generateGround(int worldSize = 100, int blockCount = 10);

  WallInfo& getInfo(unsigned id);

  void replaceWall(WallInfo info); // this isn't a reference because it is destructive
  void resetWall(WallInfo& info);// call when it's time to delete bricks and rebuild walls
  void deleteWall(unsigned id);

  unsigned getWorldSize();
  const std::vector<int>& getGround();

  void setPowerupDropRate(float percentage);

  int gridAmount = 11;
  vec3 BLOCK_SIZE;                         // standard size of brick

private:
  /////***** Private methods *****/////

  EnvironmentManager();


  /////***** Private data *****/////

  std::vector<WallInfo> walls_;            // all walls in world
  std::queue<int> debrisCache_;            // cache of broken walls waiting to be cleaned up

  unsigned worldWidth_ = 200;              // width of world TODO(Cody: add ground to manager)
  unsigned worldLength_ = 200;             

  static Grid* grid_;                      // Ray-I stuff 
  static EnvironmentManager* environment_; // this environment

  std::vector<int> groundObjs_; // collection of all blocks that make up the ground - every block had the same width,height and depth

  float powerupDropRate = 0.05f;
  void dropPowerup(vec3 position);

  int buildBuilding_ = 4; // every X walls build a building
};

#endif