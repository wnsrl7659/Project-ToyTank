/**
 * @file InputDevices.cpp
 *
 * @brief It's an implementation of InputDevices.h
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include "InputDevices.h"
#include "serviceLocator.h" // log

namespace PAEngine
{
  // InputDevice: Base Class
  InputDevice::InputDevice(TypeCommandMap* _pKeyMap) : pKeyMap(_pKeyMap), time(0.f), wasConnected(false), isConnected(false)
  {
  }

  InputDevice::~InputDevice()
  {
    activeKeyMap.clear();
    pKeyMap = nullptr;
  }

  void InputDevice::ClearKeyMap()
  {
    // clear key map
    for (auto x : *pKeyMap)
    {
      if (x.second != nullptr)
      {
        delete x.second;
        x.second = nullptr;
      }
    }

    pKeyMap->clear();
  }

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

    // Keyboard and Mouse
  KeyboardAndMouse::KeyboardAndMouse(TypeCommandMap* _pKeyMap) : InputDevice(_pKeyMap), m_mouseX(0), m_mouseY(0)
  {
    this->m_currentState.fill(false);
    this->m_previousState.fill(false);
  }

  KeyboardAndMouse::~KeyboardAndMouse()
  {
  }

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  // Gamepad
  Gamepad::Gamepad(const unsigned int playerID, TypeCommandMap* _pKeyMap) : m_playerID(playerID), InputDevice(_pKeyMap)
  {
    // Fills a block of memory with zeros.
    ZeroMemory(&m_currentState, sizeof(XINPUT_STATE));
    ZeroMemory(&m_previousState, sizeof(XINPUT_STATE));
    ZeroMemory(&m_vibration, sizeof(XINPUT_VIBRATION));

    // create the axes vectors
    m_pThumbStickLeft = new Util::Vec2F();
    m_pThumbStickRight = new Util::Vec2F();
  }
  Gamepad::~Gamepad()
  {
    delete m_pThumbStickLeft;
    delete m_pThumbStickRight;
  }

  void Gamepad::ClearKeyMap()
  {
    // clear key map
    for (auto x : *pKeyMap)
    {
      if (x.second != nullptr)
      {
        // DO NOT DELETE, it's from "gamepadKeyMapInCommon" in InputHandler.h
        // delete x.second;
        x.second = nullptr;
      }
    }

    pKeyMap->clear();
  }

  const bool Gamepad::SaveAndLoadState()
  {
    CopyMemory(&m_previousState, &m_currentState, sizeof(XINPUT_STATE));
    ZeroMemory(&m_currentState, sizeof(XINPUT_STATE));

    wasConnected = isConnected;
    isConnected = (XInputGetState(m_playerID, &m_currentState) == ERROR_SUCCESS);

    return isConnected;
  }

  // button functions
  const bool Gamepad::IsPressed(const WORD button) const
  {
    return (m_currentState.Gamepad.wButtons & button) != 0;
  }
  const bool Gamepad::WasPressed(const WORD button) const
  {
    return (m_previousState.Gamepad.wButtons & button) != 0;
  }
  const KeyState Gamepad::GetButtonState(const WORD button) const
  {
    if (WasPressed(button))
      if (IsPressed(button))
        return KeyState::StillPressed;
      else
        return KeyState::JustReleased;
    else
      if (IsPressed(button))
        return KeyState::JustPressed;
      else
        return KeyState::StillReleased;
  }

  // trigger functions
  const KeyState Gamepad::GetDigitalTriggerState(const unsigned int i)
  {
    // right trigger button
    if (i == 1)
    {
      if (m_previousState.Gamepad.bRightTrigger > TRIGGER_PRESS_SENSITIVITY)
        if (m_currentState.Gamepad.bRightTrigger > TRIGGER_PRESS_SENSITIVITY)
          return KeyState::StillPressed;
        else
          return KeyState::JustReleased;
      else
        if (m_currentState.Gamepad.bRightTrigger > TRIGGER_PRESS_SENSITIVITY)
          return KeyState::JustPressed;
        else
          return KeyState::StillReleased;
    }
    // left trigger button
    else
    {
      if (m_previousState.Gamepad.bLeftTrigger > TRIGGER_PRESS_SENSITIVITY)
        if (m_currentState.Gamepad.bLeftTrigger > TRIGGER_PRESS_SENSITIVITY)
          return KeyState::StillPressed;
        else
          return KeyState::JustReleased;
      else
        if (m_currentState.Gamepad.bLeftTrigger > TRIGGER_PRESS_SENSITIVITY)
          return KeyState::JustPressed;
        else
          return KeyState::StillReleased;
    }
  }
  const float Gamepad::GetAnalogZL() const
  {
    return (float)m_currentState.Gamepad.bLeftTrigger / 255.0f;
  }
  const float Gamepad::GetAnalogZR() const
  {
    return (float)m_currentState.Gamepad.bRightTrigger / 255.0f;
  }
  const float Gamepad::GetRelativeZL() const
  {
    return ((float)(m_currentState.Gamepad.bLeftTrigger - m_previousState.Gamepad.bLeftTrigger) / 255.0f);
  }
  const float Gamepad::GetRelativeZR() const
  {
    return ((float)(m_currentState.Gamepad.bRightTrigger - m_previousState.Gamepad.bRightTrigger) / 255.0f);
  }

  // thumb sticks
  const float Gamepad::GetRightThumbX() const
  {
    return m_pThumbStickRight->x;
  }
  const float Gamepad::GetRightThumbY() const
  {
    return m_pThumbStickRight->y;
  }
  const float Gamepad::GetLeftThumbX() const
  {
    return m_pThumbStickLeft->x;
  }
  const float Gamepad::GetLeftThumbY() const
  {
    return m_pThumbStickLeft->y;
  }

  // battery level
  const BYTE Gamepad::GetBatteryLevel() const
  {
    // get battery level
    return m_battery.BatteryLevel;
  }

  void Gamepad::Vibrate(unsigned int leftSpeed, unsigned int rightSpeed)
  {
    if (leftSpeed < 0)
      leftSpeed = 0;
    if (rightSpeed < 0)
      rightSpeed = 0;

    if (leftSpeed > m_maxMotorSpeed)
      leftSpeed = m_maxMotorSpeed;
    if (rightSpeed > m_maxMotorSpeed)
      rightSpeed = m_maxMotorSpeed;

    ZeroMemory(&m_vibration, sizeof(XINPUT_VIBRATION));
    m_vibration.wLeftMotorSpeed = (WORD)leftSpeed;
    m_vibration.wRightMotorSpeed = (WORD)rightSpeed;

    XInputSetState(m_playerID, &m_vibration);
  }
  void Gamepad::Vibrate(float normalizedLeftSpeed, float normalizedRightSpeed)
  {
    if (this == nullptr)
      return;

    if (normalizedLeftSpeed > 1.0f)
      normalizedLeftSpeed = 1.0f;
    else if (normalizedLeftSpeed < 0.0f)
      normalizedLeftSpeed = 0.0f;

    if (normalizedRightSpeed > 1.0f)
      normalizedRightSpeed = 1.0f;
    else if (normalizedRightSpeed < 0.0f)
      normalizedRightSpeed = 0.0f;

    Vibrate((unsigned int)(normalizedLeftSpeed * m_maxMotorSpeed), (unsigned int)(normalizedRightSpeed * m_maxMotorSpeed));
  }

  // acquire input
  void Gamepad::Poll()
  {
    if (m_previousState.dwPacketNumber == m_currentState.dwPacketNumber)
      // there was no change
      return;

    // get axes
    m_pThumbStickLeft->x = (float)m_currentState.Gamepad.sThumbLX;
    m_pThumbStickLeft->y = (float)m_currentState.Gamepad.sThumbLY;
    m_pThumbStickRight->x = (float)m_currentState.Gamepad.sThumbRX;
    m_pThumbStickRight->y = (float)m_currentState.Gamepad.sThumbRY;

    // get relative axis between -1.0f and 1.0f while considering a dead zone of 10%

    // left thumb stick

    // if within deadzone, set to 0
    if (m_pThumbStickLeft->getSquareLength() < m_deadzone * m_deadzone)
      m_pThumbStickLeft->x = m_pThumbStickLeft->y = 0;
    else
    {
      // calculate the percentage between the deadzone and the maximal values
      float percentage = (m_pThumbStickLeft->getLength() - m_deadzone) / (m_maxValue - m_deadzone);

      // normalize vector and multiply to get the correct final value
      m_pThumbStickLeft->normalize(m_pThumbStickLeft->length);
      *m_pThumbStickLeft *= percentage;

      if (m_pThumbStickLeft->x > 1.0f)
        m_pThumbStickLeft->x = 1.0f;
      if (m_pThumbStickLeft->x < -1.0f)
        m_pThumbStickLeft->x = -1.0f;

      if (m_pThumbStickLeft->y > 1.0f)
        m_pThumbStickLeft->y = 1.0f;
      if (m_pThumbStickLeft->y < -1.0f)
        m_pThumbStickLeft->y = -1.0f;
    }

    // right thumb stick

    // if within deadzone, set to 0
    if (m_pThumbStickRight->getSquareLength() < m_deadzone * m_deadzone)
      m_pThumbStickRight->x = m_pThumbStickRight->y = 0;
    else
    {
      // calculate the percentage between the deadzone and the maximal values
      float percentage = (m_pThumbStickRight->getLength() - m_deadzone) / (m_maxValue - m_deadzone);

      // normalize vector and multiply to get the correct final value
      m_pThumbStickRight->normalize(m_pThumbStickRight->length);
      *m_pThumbStickRight *= percentage;

      if (m_pThumbStickRight->x > 1.0f)
        m_pThumbStickRight->x = 1.0f;
      if (m_pThumbStickRight->x < -1.0f)
        m_pThumbStickRight->x = -1.0f;

      if (m_pThumbStickRight->y > 1.0f)
        m_pThumbStickRight->y = 1.0f;
      if (m_pThumbStickRight->y < -1.0f)
        m_pThumbStickRight->y = -1.0f;
    }
  }
  bool Gamepad::CheckConnection()
  {
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    if (XInputGetState(m_playerID, &state) == ERROR_SUCCESS)
      return true;

    return false;
  }

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
}