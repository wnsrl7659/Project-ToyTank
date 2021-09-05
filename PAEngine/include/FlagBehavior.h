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

/***************************************************************************/
/*  Forward declarations                                                   */
class ChangeT; //  movement controller

class FlagBehavior : public Behavior {
public:
    //constructor should be used to add functions to the base class's function lists
    FlagBehavior();
    ~FlagBehavior();

    //  use Create and Destroy to initialize and clear values specific to each Flag
    void Create(int parentID);
    void Destroy();
    bool finishGame = true;

    void DecrementHP();
    int GetHP();
    void SetPlayer(int p);
    int GetPlayer();
    void IncrementTime(int player);
    int GetTime(int player);
    int GetWinTime();
    void SetFinish();
    inline void SetIndicatorID(const int _id) { indicatorID = _id; }
    int red = 0;
    int blue = 0;
    int yellow = 0;
    int green = 0;
    float redPercent = 0;
    float bluePercent = 0;
    float yellowPercent = 0;
    float greenPercent = 0;
    float grayPercent = 0;
    void ChangeColors(int attacker, int attacked);
    bool fin3 = false;

private:
  int HP = 0;
  int player = -1;
  float time1 = 0;
  float time2 = 0;
  float time3 = 0;
  float time4 = 0;
  float winTime = 5 * 60 * 60; // sec * frame * sec
  bool fin = false;
  bool fin2 = false;


    bool count = false;
    bool black = true;
    int timer = -1;
    int speed = 7;
    int index = 0;
    // States used in the state machine
    enum FlagBehaviorState { // change anything you need to, might need to move to public
        FBS_INVALID = -1,
        FBS_NEUTRAL,
        FBS_HELD


    };

    //  private data members
    PAEngine::Gamepad* gamepad_;
    ChangeT* forces_; //  used to move Flags transform
    int indicatorID;

    friend class FlagBehavStateDefault;

    void flagUpdate();
    void ColorUpdate();
    int getMax();
    void Ending();
};