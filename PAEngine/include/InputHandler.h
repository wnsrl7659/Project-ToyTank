/**
 * @file InputHandler.h
 *
 * @brief It has integrated functions for the entire Input System.
 *        Init each devices, update each devices, etc.
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

#include "InputDevices.h"

namespace PAEngine
{

  // the main input handler class
  class InputHandler
  {
  public:
    // input devices
    bool activeMouse;						// true iff mouse input is active
    bool activeKeyboard;					// true iff keyboard input is active

    // constructor and destructor
    InputHandler();
    virtual ~InputHandler();

    // initialization
    virtual void SetDefaultKeyMap() = 0; // set up default controls

    // acquire input
    void AcquireInput(const double deltaTime);

    ///////////////////////////////////////////////////////////////////////
    // KEYBOARD AND MOUSE /////////////////////////////////////////////////

    void ClearKeyMapKBM();

    KeyboardAndMouse* GetKeyboardAndMouse();

    // get position of the mouse
    inline const long GetMouseX() const { return m_pKBM->m_mouseX; };
    inline const long GetMouseY() const { return m_pKBM->m_mouseY; };

    inline const bool IsMouseActive() const { return activeMouse; }
    inline const bool IsKeyboardActive() const { return activeKeyboard; }

    ///////////////////////////////////////////////////////////////////////
    // XInput /////////////////////////////////////////////////////////////

    TypeCommandMap gamepadKeyMapInCommon;

    void ClearKeyMapGamepadInCommon();
    void ClearKeyMapGamepads();

    void SetKeyMapToPlayer(const unsigned int playerID);

    Gamepad* GetGamepad(const unsigned int playerID);
    std::vector<Gamepad*>& GetGamePads();

    inline const bool IsGamepadActive() const { return m_gamepads.size() ? true : false; }

  private:

    ///////////////////////////////////////////////////////////////////////
    // KEYBOARD AND MOUSE /////////////////////////////////////////////////

    KeyboardAndMouse* m_pKBM;
    TypeCommandMap* m_pKBMKeyMap;

    void GetKeyboardAndMouseState();							                // gets the keyboard and mouse button state, uses GetAsyncKeyState to read the state of all 256 keys
    const KeyState GetKeyState(const unsigned int keyCode) const;	// gets the state of the specified key, depending on its state in the previous and the current frame
    const bool IsPressed(int keyCode) const;						          // returns true iff the key is down
    void UpdateKBMkeyMap(TypeCommandMap* keyMap, TypeCommandActiveMap& activeKeyMap);

    ///////////////////////////////////////////////////////////////////////
    // XINPUT /////////////////////////////////////////////////////////////

    std::vector<Gamepad*> m_gamepads;	                      // an array of all available XInput controllers
    Gamepad* m_pGamepadPointerArray[XUSER_MAX_COUNT];       // m_gamepads to ARRAY for "easy access"
    TypeCommandMap* m_pGamepadKeyMapArray[XUSER_MAX_COUNT]; // it store each player's control set

    unsigned int m_gamepadsNum;		                    // the number of connected gamepads
    unsigned int m_playersNum;		                    // the number of players

    const bool InitializeXInputGamepads();
    Gamepad* AddNewGamepad(const int _playerID);
    void DeleteTargetGamepad(const int _playerID);
    void UpdateGamepadkeyMap(TypeCommandMap* keyMap, TypeCommandActiveMap& activeKeyMap, const int currentPlayerOffset = 0);

  };
}