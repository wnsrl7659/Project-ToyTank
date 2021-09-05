/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/
#pragma once
#pragma once
#include "Behavior.h"
#include "InputDevices.h" // gamepad
#include "Grid.h"
#include <utility>

/***************************************************************************/
/*  Forward declarations                                                   */
class ChangeT; //  movement controller

class AstarBehavior : public Behavior {
public:
    //constructor should be used to add functions to the base class's function lists
    AstarBehavior();
    ~AstarBehavior();

    //  use Create and Destroy to initialize and clear values specific to each Astar
    void Create(int parentID);
    void Update(float dt) override;
    void Destroy();
    bool finishGame = true;

    bool set = false;
    int myIndex;
    int targetIndex;
    Grid::Node* myNode;

    std::priority_queue<std::pair<int, int>> PQ;

    bool findInitial = false;
    bool found = false;
    bool setTarget = false;
    float targetX;
    float targetZ;
    bool initiate = false;
    bool moving = false;
    int owner = -1;
    int hit = -1;
    int index = -1;

    int color = -1;
    int timer = 0;
    const int speed = 10;
    bool black = true;
    bool count = false;

    int particleID;
    float ParticleTimer = 0;

    void ToBlackWhite(bool b);
    void ToWhite();
    int ToOwnerColor();
    void ChangeColor();
    vec3 targetcolor = vec3(1, 1, 1);
    vec3 oldcolor = vec3(1, 1, 1);
    vec3 currentcolor = vec3(1, 1, 1);
    bool changingColor = false;

private:

    int HP = 0;
    int player = -1;
    float time1 = 0;
    float time2 = 0;
    float time3 = 0;
    float time4 = 0;
    float winTime = 1200;
    bool fin = false;

    int curIndex = 55;

    // States used in the state machine
    enum AstarBehaviorState { // change anything you need to, might need to move to public
        FBS_INVALID = -1,
        FBS_NEUTRAL,
        FBS_HELD


    };

    //  private data members
    PAEngine::Gamepad* gamepad_;
    ChangeT* forces_; //  used to move Astars transform

    friend class AstarBehavStateDefault;

    void astarUpdate();
    void astarColorUpdate();
    void groundUpdate();
    int getIndex(float xPos, float yPos);
    void astarMove(float tx, float tz);
    void Ending();


};