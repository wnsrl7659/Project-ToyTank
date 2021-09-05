/**
 * @file InputHandler.cpp
 *
 * @brief It's an implementation of InputHandler.h
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include "InputHandler.h"
#include "InputConnectionHandler.h"
#include "serviceLocator.h" // log

 // XInput
#include <wbemidl.h>

// MSDN macros
// ref. https://docs.microsoft.com/en-us/windows/win32/medfound/saferelease
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

#define TERMINATING_CONNECTION_TIME 5.f

namespace PAEngine
{
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  // Input Handler
  InputHandler::InputHandler() : m_gamepadsNum(0), m_playersNum(0)
  {
    // initialize keyboard and mouse
    m_pKBMKeyMap = new TypeCommandMap();
    m_pKBM = new KeyboardAndMouse(m_pKBMKeyMap);
    activeMouse = true;
    activeKeyboard = true;

    for (int i = 0; i < XUSER_MAX_COUNT; ++i)
    {
      m_pGamepadPointerArray[i] = nullptr;
      m_pGamepadKeyMapArray[i] = nullptr;
    }

    // look up connected gamepads then initialize XInput gamepads
    InitializeXInputGamepads();

    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "The input handler was successfully initialized.");
  };

  InputHandler::~InputHandler()
  {
    // delete the pointer to the keyboard and mouse class
    m_pKBM->ClearKeyMap();
    delete m_pKBM;
    delete m_pKBMKeyMap;

    ClearKeyMapGamepadInCommon();

    // release gamepads
    if (IsGamepadActive())
    {
      ClearKeyMapGamepads();

      for (int i = 0; i < XUSER_MAX_COUNT; ++i)
      {
        // clear Gamepad Array
        m_pGamepadPointerArray[i] = nullptr;

        // release gamepads' keyMap
        if (m_pGamepadKeyMapArray[i] != nullptr)
        {
          delete m_pGamepadKeyMapArray[i];    // release
          m_pGamepadKeyMapArray[i] = nullptr; // clear
        }
      }
    }

    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, "The input handler was shutdown successfully.");
  }

  void InputHandler::AcquireInput(const double deltaTime)
  {
    /////////////////////
    // keyboard and mouse
    if (activeKeyboard || activeMouse)
    {
      GetKeyboardAndMouseState();   // get keyboard and mouse state
      m_pKBM->activeKeyMap.clear(); // clear the previous key data
      UpdateKBMkeyMap(m_pKBM->pKeyMap, m_pKBM->activeKeyMap);
    }

    ///////////
    // gmaepads
    for (int i = 0; i < XUSER_MAX_COUNT; ++i)
    {
      Gamepad* pGamepad = m_pGamepadPointerArray[i];
      bool isConnected = false;

      ///////////////////////
      // new connection check
      if (pGamepad == nullptr)
      {
        pGamepad = AddNewGamepad(i);

        // still no connection, skip
        if (pGamepad == nullptr)
          continue;
        else
        {
          std::string s = "Connection during the gameplay is confirmed.";
          Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, s);

          DeviceConnected(pGamepad); // its for managing other components' ref
        }
      }

      /////////////////////////
      // still connection check

      // connection check
      // set prevState and get currState
      isConnected = pGamepad->SaveAndLoadState();

      // reconnection
      if (!pGamepad->wasConnected && pGamepad->isConnected)
      {
        std::string s;
        s = "P" + std::to_string(pGamepad->GetPlayerID() + 1);

        if (pGamepad->time > 0.f)
        {
          pGamepad->time = 0.f;
          s += " is reconnected.";
        }
        else
          s += " is connected.";

        Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, s);
      }
      
      // still not reconnection
      if (!pGamepad->wasConnected && !pGamepad->isConnected)
      {
        if (pGamepad->time == 0.f)
        {
          std::string s;
          s = "P" + std::to_string(pGamepad->GetPlayerID() + 1);
          s += " is disconnected! This gamepad data will be deleted in " + std::to_string(TERMINATING_CONNECTION_TIME) + "s";
          Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, s);
        }
        else if (pGamepad->time > TERMINATING_CONNECTION_TIME)
        {
          DeleteTargetGamepad(pGamepad->GetPlayerID());

          pGamepad->time = 0.f;
        }

        // track the disconnected time
        pGamepad->time += deltaTime;
      }
      // connecting
      else
      {
        pGamepad->Poll();               // get gamepad's each input state
        pGamepad->activeKeyMap.clear(); // clear the previous key data
        UpdateGamepadkeyMap(pGamepad->pKeyMap, pGamepad->activeKeyMap, i);
      }
    }
  }

  void InputHandler::UpdateKBMkeyMap(TypeCommandMap* keyMap, TypeCommandActiveMap& activeKeyMap)
  {
    bool isActive = false;

    // loop through the map of all possible actions and find the active key bindings
    for (auto x : *keyMap)
    {
      if (x.second->m_chord.empty())
        continue;

      isActive = true;
      for (auto y : x.second->m_chord)
      {
        if (y.m_keyCode >= 0 && y.m_keyCode)
        {
          // this is a keyboard or mouse chord
          if (GetKeyState(y.m_keyCode) != y.m_keyState)
          {
            isActive = false;
            break;
          }
        }
      }

      if (isActive)
        activeKeyMap.insert(std::pair<EventType, GameCommand&>(x.first, *x.second));
    }
  }

  void InputHandler::UpdateGamepadkeyMap(TypeCommandMap* keyMap, TypeCommandActiveMap& activeKeyMap, const int currentPlayerOffset)
  {
    bool isActive = false;
    Gamepad* pCurrentGamepad = m_pGamepadPointerArray[currentPlayerOffset];

    // loop through the map of all possible actions and find the active key bindings
    for (auto x : *keyMap)
    {
      if (x.second->m_chord.empty())
        continue;

      // test chord
      isActive = true;
      for (auto y : x.second->m_chord)
      {
        // right trigger
        if (y.m_keyCode == VK_PAD_RTRIGGER)
        {
          if (pCurrentGamepad->GetDigitalTriggerState(1) != y.m_keyState)
          {
            isActive = false;
            break;
          }
        }
        // left trigger
        else if (y.m_keyCode == VK_PAD_LTRIGGER)
        {
          if (pCurrentGamepad->GetDigitalTriggerState(0) != y.m_keyState)
          {
            isActive = false;
            break;
          }
        }
        // other gamepad chord
        else
        {
          if (pCurrentGamepad->GetButtonState((WORD)(y.m_keyCode)) != y.m_keyState)
          {
            isActive = false;
            break;
          }
        }
      }

      if (isActive)
        activeKeyMap.insert(std::pair<EventType, GameCommand&>(x.first, *x.second));
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////// Keyboard and Mouse ////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////

  void InputHandler::GetKeyboardAndMouseState()
  {
    if (activeKeyboard || activeMouse)
    {
      // store the old keyboard state
      m_pKBM->m_previousState = m_pKBM->m_currentState;

      // read the current keyboard state
      for (int i = 0; i < 256; i++)
      {
        m_pKBM->m_currentState[i] = IsPressed(i);

        if (IsPressed(i))
          int i = 0;
      }
    }

    if (activeMouse)
    {
      // get the position of the mouse cursor
      POINT cursorPos;
      GetCursorPos(&cursorPos);
      m_pKBM->m_mouseX = cursorPos.x;
      m_pKBM->m_mouseY = cursorPos.y;
    }
  }

  const KeyState InputHandler::GetKeyState(const unsigned int keyCode) const
  {
    if (m_pKBM->m_previousState[keyCode] == 1)
      if (m_pKBM->m_currentState[keyCode] == 1)
        return KeyState::StillPressed;
      else
        return KeyState::JustReleased;
    else
      if (m_pKBM->m_currentState[keyCode] == 1)
        return KeyState::JustPressed;
      else
        return KeyState::StillReleased;
  }

  inline const bool InputHandler::IsPressed(int keyCode) const
  {
    return (GetAsyncKeyState(keyCode) & 0x8000) ? 1 : 0;
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////// XInput / Gamepad //////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////

  // initialize XInput gamepads
  // currently only one player is supported
  const bool InputHandler::InitializeXInputGamepads()
  {
    m_pGamepadPointerArray[0] = nullptr;
    m_pGamepadPointerArray[1] = nullptr;
    m_pGamepadPointerArray[2] = nullptr;
    m_pGamepadPointerArray[3] = nullptr;

    // try to get a gamepad for each player
    for (unsigned int p = 0; p < XUSER_MAX_COUNT; p++)
      AddNewGamepad(p);

    if (m_gamepadsNum != 0)
      return true;

    return false;
  }

  Gamepad* InputHandler::AddNewGamepad(const int _playerID)
  {
    // get port
    int playerID = -1;
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    // check whether it is connected ot not
    if (XInputGetState(_playerID, &state) == ERROR_SUCCESS)
      playerID = _playerID;

    if (playerID != -1)
    {
      // check key setting for this player
      if (m_pGamepadKeyMapArray[playerID] == nullptr)
      {
        m_pGamepadKeyMapArray[playerID] = new TypeCommandMap(); // allocate
        SetKeyMapToPlayer(playerID);                            // bring the default key setting
      }
      else
      {
        std::string s;
        s = "P" + std::to_string(playerID + 1);
        s += " was deleted and reconnected! (The Gameped object was deleted, but there is a remained KeyMap data.)";
        Util::ServiceLocator::getLogger()->print(Util::SeverityType::warning, s);
      }

      // create gamepad
      Gamepad* pGamepad = new Gamepad(playerID, m_pGamepadKeyMapArray[playerID]);
      m_gamepads.push_back(pGamepad);
      m_pGamepadPointerArray[playerID] = *(m_gamepads.end() - 1);

      std::string s = "Gamepad Init: Player " + std::to_string(playerID + 1) + " conection confirmed.";
      s += " A new Gamepad object is added in Input System.";
      Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, s);

      // to manage the whole gamepads (InputHandler's member variables)
      m_gamepadsNum = (unsigned int)m_gamepads.size();
      m_playersNum = m_gamepadsNum;
      pGamepad = nullptr;

      return m_pGamepadPointerArray[playerID];
    }

    return nullptr;
  }

  void InputHandler::DeleteTargetGamepad(const int _playerID)
  {
    // release
    Gamepad* target = *(m_gamepads.begin() + _playerID);
    DeviceDisconnected(target); // its for managing other components' ref
    delete target;
    target = nullptr;

    // clear
    m_gamepads.erase(m_gamepads.begin() + _playerID);
    m_pGamepadPointerArray[_playerID] = nullptr;

    std::string s;
    s = "P" + std::to_string(_playerID + 1);
    s += "'s gamepad data is deleted.";
    Util::ServiceLocator::getLogger()->print(Util::SeverityType::info, s);

    --m_gamepadsNum;
    --m_playersNum;
  }

  ///////////////////////////////////////////////////////////////////////
  // KEYBOARD AND MOUSE /////////////////////////////////////////////////

  void InputHandler::ClearKeyMapKBM()
  {
    m_pKBM->ClearKeyMap();
  }

  KeyboardAndMouse* InputHandler::GetKeyboardAndMouse()
  {
    if (!activeKeyboard)
      Util::ServiceLocator::getLogger()->print(Util::SeverityType::error, "There is no connected keyboard!");

    return m_pKBM;
  }

  ///////////////////////////////////////////////////////////////////////
  // XInput /////////////////////////////////////////////////////////////

  void InputHandler::ClearKeyMapGamepadInCommon()
  {
    // clear key map
    for (auto x : gamepadKeyMapInCommon)
    {
      if (x.second != nullptr)
      {
        delete x.second;
        x.second = nullptr;
      }
    }

    gamepadKeyMapInCommon.clear();
  }

  void InputHandler::ClearKeyMapGamepads()
  {
    // for each gamepad's keymap
    std::vector<Gamepad*>::iterator it = m_gamepads.begin();
    while (it != m_gamepads.end())
    {
      (*it)->ClearKeyMap();
      ++it;
    }
  }

  void InputHandler::SetKeyMapToPlayer(const unsigned int playerID)
  {
    TypeCommandMap* targetKeyMap = m_pGamepadKeyMapArray[playerID];
    Gamepad* targetGamepad = m_pGamepadPointerArray[playerID];

    // verification check
    if (targetKeyMap == nullptr)
    {
      std::string s;
      s = "ERROR! P" + std::to_string(playerID);
      s += " is not available. (targetKeyMap is nullptr)";
      Util::ServiceLocator::getLogger()->print(Util::SeverityType::error, s);

      return;
    }

    // if it's not a new player, clear data
    if (targetGamepad != nullptr)
      m_pGamepadPointerArray[playerID]->ClearKeyMap();

    // bring the default key setting
    for (TypeCommandMap::iterator it = gamepadKeyMapInCommon.begin(); it != gamepadKeyMapInCommon.end(); ++it)
      targetKeyMap->insert(*it);
  }

  // PlayerID: 0~3
  Gamepad* InputHandler::GetGamepad(const unsigned int playerID)
  {
    return m_pGamepadPointerArray[playerID];
  }

  std::vector<Gamepad*>& InputHandler::GetGamePads()
  {
    //if (!IsGamepadActive())
    //  Util::ServiceLocator::getLogger()->print(Util::SeverityType::error, "There is no connected gamepad!");

    return m_gamepads;
  }
}