/******************************************************************************
  file: Bahavior.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
#include <vector>
#include <functional>
#include "Component.h"
#include "BehaviorManager.h"

class BehaviorState;
enum BehaviorType;

class Behavior : public Component {
public:
    Behavior();
    ~Behavior();

    /*  Initialize the Behaviors base class, allocating the memory for the    *
     *  derived class if needed                                               */
    static Behavior* Init(int parentID_, BehaviorType bType);
    /*  Updates the behavior using the current states update function and     *
     *  updates the timer                                                     */
    virtual void Update(float dt);
    /*  Clears data in the base class, but does not free memory so it can be  *
     *  Initialized again for another object                                  */
    void Shutdown();

    virtual void Destroy() override {};

    /*  Changes the behaviors state, calling state functions to shutdown the  *
     *  current state and initialize the next state.                          */
    void ChangeState(unsigned nextState);

    /*  returns the current value of the timer  *
     *  (time in seconds since last reset)      */
    float& GetTimer();

    float GetTimerConst() const;

    int GetParentID() const;

    BehaviorType GetType() const;

    void AddBehaviorState(BehaviorState* newState);
private:
    BehaviorType behavType_;
    int currentState_;
    std::vector<BehaviorState*> behavStates_;
    float timer_;

    friend class BehaviorManager;
};