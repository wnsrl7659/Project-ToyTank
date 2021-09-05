/**
 * @file InputDevices.h
 *
 * @brief It has each type of available devices and its button system
 *        + currentState, previousState, key map, active key map
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

#include <Windows.h> // MUST be on the top of the file (upper than Xinput.h)

#include "EventSystem.h" // EventType
#include "InputType.h"   // KeyState
#include "Vectors.h"	 // custom Vec2F

 // containers
#include <unordered_map>
#include <array>

// XInput
#include <Xinput.h>
#pragma comment (lib, "xinput.lib")

namespace PAEngine
{
  ///////////////////////////////////////////////////////////////////////////
  // Input Devices : (1) Keyboard and mouse, (2) XBox Controller ////////////

  typedef std::unordered_multimap<EventType, GameCommand&> TypeCommandActiveMap;
  typedef std::unordered_multimap<EventType, GameCommand*> TypeCommandMap;
  typedef std::pair<EventType, GameCommand*> TypeCommandPair;

  class InputDevice
  {
  public:
    TypeCommandActiveMap activeKeyMap;
    TypeCommandMap* pKeyMap;
    float time;
    bool isConnected;
    bool wasConnected;

    InputDevice(TypeCommandMap* _pKeyMap);
    ~InputDevice(); // for (auto x : keyMap) keyMap.clear();

    void ClearKeyMap();

    friend class InputHandler;
  };

  // the keyboard and mouse class
  class KeyboardAndMouse : public InputDevice
  {
  public:
    KeyboardAndMouse(TypeCommandMap* _pKeyMap);
    ~KeyboardAndMouse();

    friend class InputHandler;

  private:
    std::array<bool, 256> m_currentState;					// the state of the keyboard keys and mouse buttons in the current frame
    std::array<bool, 256> m_previousState;				// the state of the keyboard keys and mouse buttons in the previous frame

    long m_mouseX, m_mouseY;								// the position of the mouse cursor
  };

  // the gamepad class (XInput)
  class Gamepad : public InputDevice
  {
  public:

    // constructor and destructor
    Gamepad(const unsigned int, TypeCommandMap* _pKeyMap);
    ~Gamepad();

    void ClearKeyMap();

    const bool SaveAndLoadState();
    inline const unsigned int GetPlayerID() {
      if (this == nullptr) {
        //Util::ServiceLocator::getLogger()->print(
        //  Util::SeverityType::warning,
        //  "Null pointer Gamepad!");
        return 0; }
      return m_playerID; }

    // button functions
    const KeyState GetButtonState(const WORD button) const;	// returns the state of a button
    const bool IsPressed(const WORD button) const;			// returns true iff the button is pressed
    const bool WasPressed(const WORD button) const;			// returns true iff the button was pressed in the previous frame

    // trigger functions
    const KeyState GetDigitalTriggerState(const unsigned int i = 0); // returns the state of the left (0) or right (1) trigger
    const float GetAnalogZL() const;	// returns how far the trigger button is pressed in normalized values
    const float GetAnalogZR() const;
    const float GetRelativeZL() const;	// returns how far the trigger button is pressed relative to the last frame
    const float GetRelativeZR() const;

    // thumb sticks
    const float GetRightThumbX() const;
    const float GetRightThumbY() const;
    const float GetLeftThumbX() const;
    const float GetLeftThumbY() const;

    // battery level
    const BYTE GetBatteryLevel() const;

    // vibrate the gamepad
    void Vibrate(unsigned int, unsigned int);
    void Vibrate(float, float);

    friend class InputHandler;

  private:
    const float TRIGGER_PRESS_SENSITIVITY = 150.f;

    XINPUT_STATE m_currentState;		// the state of the gamepad in the current frame
    XINPUT_STATE m_previousState;		// the state of the game in the previous frame
    const unsigned int m_playerID;	// player number (0-3)

    // vector and deadzone
    const float m_maxValue = 32767;		// maximal value for the axes
    const float m_deadzone = 6552;		// deadzone of 20%
    Util::Vec2F* m_pThumbStickLeft;	// left thumb stick vector
    Util::Vec2F* m_pThumbStickRight;	// right thumb stick vector

    // vibration
    XINPUT_VIBRATION m_vibration;					// vibration settings
    const unsigned int m_maxMotorSpeed = 65535;	// max motor speed

    // battery
    XINPUT_BATTERY_INFORMATION m_battery;			// information about the battery of the gamepad

    // acquire the state of the gamepad
    void Poll();							// get gamepad state
    bool CheckConnection();						// check whether the gamepad for the specified player is still connected
  };
}