#include "pch.h"
using std::vector;

#include "EnvironmentManager.h"
#include "ComponentManager.h"
#include "Physics.h"
#include "Transform.h"
#include "ModelManager.h"
#include "ModelComponent.h"
#include "ObjectFactory.h"
#include "ObjectManager.h"
#include "EventSystem.h"
#include "BulletBehavior.h"
#include "ParticleEmitter.h"
#include <chrono>
#include <ctime>  

#define _USE_MATH_DEFINES
#include <math.h>

EnvironmentManager* EnvironmentManager::environment_ = nullptr;
Grid* EnvironmentManager::grid_ = nullptr; // Ray-I stuff 


/******************************************************************************
* Bullet has already determined that there was a collision with this object
* and with the other specified object
* this function was specified to be called in that event - resolve the logic
* behind what this collision means
*
* input : bullet - name of the bullet that is hitting this wall
*         myID   - object ID of the collided object
*******************************************************************************/
void BulletCollidesWithBigWall_CB(unsigned myID, unsigned other, const std::string& bullet)
{
    // only destroy with explosive bullets
    GameObject* bulletObj = ObjectManager::GetObjectByID(other);
    bool explode = ObjectManager::GetObjectByID(other)->GetComponent<BulletBehavior>()->getExplode();
    if (explode)
    {
        // we're only concerned about what happens to us
        Physics* physics = ObjectManager::GetObjectByID(myID)->GetComponent<Physics>();

        int wallID = physics->box->getWallID();

        // explosion particles
        ParticleEmitter* pe = bulletObj->GetComponent<ParticleEmitter>();
        if (pe) {
            pe->SetParticleTransform({ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 });
            pe->SetTransformBehavior({ 0,0,0 }, .75, { .9,0,0,0,0,.9,0,0,0,0,.9,0,0,0,0,1 });
            pe->SetColorBehavior({ 1,0,0 }, { 1,1,0 }, 1, 0);
            pe->BurstEmit(75);
        }
        ObjectManager::DestroyObject(bulletObj->GetID(), 5);
        // we're destroying an object so an obj id is needed
        ObjectManager::DestroyObject(myID);
        // play sound for wal breaking
        EventSystem::SendEvent(ET_SOUND_OS_EXPLOSION, nullptr, 0);

        // we're replacing the big wall with all the bricks defined by this wall id so we need the id that was
        // stored in the collided object
        WallInfo info = EnvironmentManager::getEnvironment()->getInfo(wallID);
        if (info.isDestructable)
        {
            EnvironmentManager::getEnvironment()->replaceWall(info);
        }
    }
    else {
        // sound for bullet bouncing off the wall

    }


}

EnvironmentManager::EnvironmentManager()
{
    BLOCK_SIZE.x = 1;
    BLOCK_SIZE.y = 1;
    BLOCK_SIZE.z = 1;
}

void EnvironmentManager::dropPowerup(vec3 position)
{
    float rando = float(rand()) / RAND_MAX;
    if (rando < powerupDropRate)
    {
        ObjectManager* objMan = ObjectManager::GetInstance();

        int powerup = ObjectFactory::CreatePowerUp();
        objMan->GetObjectByID(powerup)->GetComponent<Physics>()->Teleport(position.x, position.y + 1, position.z);
    }
}

EnvironmentManager* EnvironmentManager::getEnvironment()
{
    environment_->init();
    return environment_;
}

Grid& EnvironmentManager::getGrid()
{
    environment_->init();
    return *grid_;
}

void EnvironmentManager::init()
{
    if (environment_ == nullptr)
    {
        environment_ = new EnvironmentManager();
        environment_->grid_ = new Grid(environment_->worldWidth_, environment_->worldLength_, environment_->BLOCK_SIZE.z);
    }
}

void EnvironmentManager::update(float dt)
{
    if (!debrisCache_.empty())
    {
        // this wall has been destroyed and is awaiting reclaimation
        WallInfo& info = getInfo(debrisCache_.front());
        info.TTL -= dt;
        //printf("\nwall #%i TTL: %f\n", info.wallNum, info.TTL);
        // reclaim bricks and reuild a new wall
        if (info.TTL <= 0)
        {
            resetWall(info);
            debrisCache_.pop();
        }
    }
}

void EnvironmentManager::shutdown()
{
    delete grid_;
    delete environment_;
    environment_ = nullptr;
    grid_ = nullptr;
}

void EnvironmentManager::restart()
{
    environment_->shutdown();
    environment_->init();
}


void PrintTime()
{
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
    std::cout << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << "\n";
}

// currently configured to hide the bricks and test collisions of the big wall with bullets
vec3 EnvironmentManager::generateWall(unsigned width, unsigned height, vec3 position, float angle, bool isBuilding, bool isDestructable,  float mass)
{
    WallInfo newInfo;
    newInfo.isValid = true;
    newInfo.width = width;
    newInfo.height = height;
    newInfo.position = position;
    newInfo.angle = angle;
    newInfo.mass = mass;
    newInfo.TTL = newInfo.startTTL;
    newInfo.isDestructable = isDestructable;
    newInfo.building = isBuilding;
    vec3 lastbrickPos;

    // keep track of these walls for collision purposes
    int foundValidWall = -1;
    for (int i = 0; i < walls_.size(); i++)
    {
        if (!walls_[i].isValid)
        {
            newInfo.wallNum = i;
            foundValidWall = i;

            break;
        }
    }

    // add info to vector as no empty spot was found
    if (foundValidWall == -1)
    {
        newInfo.wallNum = walls_.size();
    }


    // make the bricks
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // calc brick position
            vec3 temp, newPos;
            temp.x = -(j * BLOCK_SIZE.x * 2);
            float stagger = (i % 2) * (BLOCK_SIZE.y / 2);
            temp.x -= 1.75 * stagger;
            temp.y = (i * BLOCK_SIZE.y * 2);
            temp.z = 0;

            // rotate
            float angle = newInfo.angle;
            angle *= (2 * M_PI) / 360.0f;

            newPos.x = (cos(angle) * temp.x) + (sin(angle) * temp.z);
            newPos.y = temp.y;
            newPos.z = -(sin(angle) * temp.x) + (cos(angle) * temp.z);
            // newPos.z *= -1;

            newPos.x += position.x;
            newPos.y += position.y;
            newPos.z += position.z;

            if(i == height - 1 && j == width - 1)
            {
              lastbrickPos = newPos;
            }

            int blockid =
                ObjectFactory::CreateBox(BLOCK_SIZE.x, BLOCK_SIZE.y, BLOCK_SIZE.z, 1, newPos.x, newPos.y, newPos.z, MT_WALL_BRICK);
            newInfo.blocks.push_back(blockid);

            GameObject* obj = ObjectManager::GetObjectByID(blockid);

            if (!isDestructable)
            {
                ModelComponent* model = obj->GetComponent<ModelComponent>();
                size_t id = model->GetInstance();
                Rendering::IModel* m = ModelManager::GetModel(MT_WALL_BRICK);
                m->SetColor(id, 0.3f, 0.3f, 0.3f);
            }

            if (ObjectManager::IsValidID(blockid)) {
                Physics* phys = obj->GetComponent<Physics>();
                phys->SetMass(0);
            }
        }
    }

    float deg = angle;
    vec3 temp, newPos;
    temp.x = +(BLOCK_SIZE.x * 2 / 2) - BLOCK_SIZE.x * 2 * width / 2;
    temp.y = -(BLOCK_SIZE.y * 2 / 2) + BLOCK_SIZE.y * 2 * height / 2;
    temp.z = 0;

    angle *= (2 * M_PI) / 360.0f;

    newPos.x = (cos(angle) * temp.x) + (sin(angle) * temp.z);
    newPos.y = temp.y;
    newPos.z = -(sin(angle) * temp.x) + (cos(angle) * temp.z);
    //newPos.z *= -1;

    newPos.x += position.x;
    newPos.y += position.y;
    newPos.z += position.z;

    // big wall around bricks
    if (deg == 0.0f || deg == 180.0f)
    {
        newInfo.bigwall_id = ObjectFactory::CreateBox(
            BLOCK_SIZE.x * width, BLOCK_SIZE.y * height, BLOCK_SIZE.z + 0.1,
            0, newPos.x, newPos.y, newPos.z, MT_WALL_LARGE);
    }
    else if (deg == 90.0f || deg == 270.0f)
    {
        newInfo.bigwall_id = ObjectFactory::CreateBox(
            BLOCK_SIZE.x + 0.1, BLOCK_SIZE.y * height, BLOCK_SIZE.z * width,
            0, newPos.x, newPos.y, newPos.z, MT_WALL_LARGE);
    }

    // we need to set the wall id in whatever will be looking for a collision - in this case the big wall
    // this will have to be changed to every brick if they are looking for the colliison
    if (ObjectManager::IsValidID(newInfo.bigwall_id)) {
        Physics* physics = ObjectManager::GetObjectByID(newInfo.bigwall_id)->GetComponent<Physics>();
        physics->box->setWallID(newInfo.wallNum);
        OnCollision* fp = (OnCollision*)(&BulletCollidesWithBigWall_CB);
        physics->box->addCollisionCallback(fp, "bullet");

        // add info to vector as no empty spot was found
        if (foundValidWall == -1)
        {
            environment_->walls_.push_back(newInfo);
        }
        else
        {
            environment_->walls_[foundValidWall] = newInfo;
        }

      // generate the exact corners for ray
      vector<float> cornerPos =
      {
        max(min(newInfo.position.x + BLOCK_SIZE.x / 2.0f, 100.0f), -100.0f),
        newInfo.position.y + BLOCK_SIZE.y / 2.0f,
        max(min(newInfo.position.z + BLOCK_SIZE.z / 2.0f, 100.0f), -100.0f)
      };
      vector<float> topPos =
      {
        cornerPos[0] + (BLOCK_SIZE.x * width),
        cornerPos[1] + (BLOCK_SIZE.y * height),
        cornerPos[2] + (BLOCK_SIZE.z)
      };

        environment_->grid_->SetWallLocation(cornerPos, topPos);
    }

    lastbrickPos.y = -2;
    return lastbrickPos;
}

vec3 EnvironmentManager::generateRandomWall(unsigned width, unsigned height, float mass)
{
    int x = rand() % worldWidth_ - (worldWidth_ / 2); // generate a coordinate +- half the world size
    int z = rand() % worldLength_ - (worldLength_ / 2);
    int ang = (rand() % 4) * 90;

    if (!width)
        width = (rand() % 7) + 3; //[3,10]
    if (!height)
        height = (rand() % 2) + 2; //[2,4]

  return generateWall(width, height, vec3(x, -2, z), ang, mass);
}

void EnvironmentManager::generateRandomBuilding(unsigned width, unsigned length, unsigned height)
{
  if(width == 0)
  {
    width = 5 + (rand() % 5);
    length = 5 + (rand() % 5);
    height = 5 + (rand() % 10);
  }

  int x = (rand() % worldWidth_) - (worldWidth_ / 2); // generate a coordinate +- half the world size
  int z = rand() % worldLength_ - (worldLength_ / 2);
  x = max(min(x, 90),-90);
  z = max(min(z, 90),-90);
    
  vec3 start(x, -2, z);
  vec3 last = generateWall(width, height, start, 0, 0.5f);
  generateWall(width, height, last, 90, 0.5f);
  last = generateWall(width, height, start, 90, 0.5f);
  generateWall(width, height, last, 0, 0.5f);
}

void EnvironmentManager::generateGround(int worldSize, int blockCount)
{
    gridAmount = blockCount;
    worldWidth_ = worldSize;
    worldLength_ = worldSize;
    vec3 blockSize(float(worldSize) / blockCount + 02.f, 1, float(worldSize) / blockCount + 0.2f);
    blockSize.x /= 2;
    blockSize.z /= 2;
    int i = 0;
    for (size_t height = 0; height < blockCount; height++)
    {
        for (size_t width = 0; width < blockCount; width++)
        {
            vec3 position(-worldSize / 2.0f + blockSize.x + width * blockSize.x * 2, // margin
                -4.0f,
                -worldSize / 2.0f + blockSize.z + height * blockSize.z * 2);
            int ground = ObjectFactory::CreateGround(position, blockSize, i);
            groundObjs_.push_back(ground);
            ++i;
        }
    }
}

WallInfo& EnvironmentManager::getInfo(unsigned id)
{
    return walls_[id];
}

void EnvironmentManager::replaceWall(WallInfo info)
{
    unsigned width = info.width;
    unsigned height = info.height;
    vec3 position = info.position;
    float mass = info.mass;

    // put blocks in their place
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int blockid = info.blocks.back();
            info.blocks.pop_back();
            Physics* trans = ObjectManager::GetObjectByID(blockid)->GetComponent<Physics>();
            //change mass 0 to something
            trans->SetMass(mass);
        }
    }

    // add to debris cache so we can reclaim it later
    debrisCache_.push(info.wallNum);

    //printf("wall hit... TTL: %f\n", info.TTL);
    //PrintTime();
}

void EnvironmentManager::deleteWall(unsigned id)
{
    if (!walls_.empty())
    {
        //printf("deleting wall\n");
        //PrintTime();

        dropPowerup(walls_[id].position);
        walls_[id].isValid = false;
        walls_[id].blocks.clear();
        walls_[id].bigwall_id = -1;
        walls_[id].width = 0;
        walls_[id].height = 0;

    }
}

unsigned EnvironmentManager::getWorldSize()
{
    if (worldWidth_ != worldLength_)
        printf("non square world\n");
    return worldWidth_;
}

const std::vector<int>& EnvironmentManager::getGround()
{
    return groundObjs_;
}

void EnvironmentManager::setPowerupDropRate(float percentage)
{
    if (percentage >= 0 && percentage <= 1)
        powerupDropRate = percentage;
}

void EnvironmentManager::resetWall(WallInfo& info)
{
    //printf("reclaiming blocks\n");
    //PrintTime();
  // delete all the blocks the go to this wall
    for (int block : info.blocks)
    {

    ObjectManager::DestroyObject(block);
  }
  bool wasBuilding = info.building;
  // delete the info for this wall
  deleteWall(info.wallNum);

  // if we blew up a building we should relace it with a building don't you think?
  if (wasBuilding && buildBuilding_-- <= 0)
  {
    environment_->generateRandomBuilding();
    buildBuilding_ = 4;
  }
  else
    environment_->generateRandomWall();
}


