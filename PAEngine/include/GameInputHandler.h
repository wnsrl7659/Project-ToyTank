/**
 * @file GameInputDevices.h
 *
 * @brief This is the header of GameInputHandler.cpp
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

#include "InputHandler.h"

namespace PAEngine
{
  class GameInput : public InputHandler
  {
  public:
    GameInput();
    virtual void SetDefaultKeyMap() override;

    // utility functions
    const bool SetGameInput(EventType _eventType, GameCommand* _pGameCommand, EventHandler function = nullptr);
    const bool SetGameInputKBM(EventType _eventType, GameCommand* _pGameCommand, EventHandler function = nullptr);
  };
}