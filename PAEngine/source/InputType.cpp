/**
 * @file InputType.cpp
 *
 * @brief It's an implementation file of InputType.h
 *
 * @author Minsu Kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include "InputType.h"

namespace PAEngine
{
  //////////////////////////////////////////////////////////////////////////////
  // BindInfo Class ////////////////////////////////////////////////////////////

  BindInfo::BindInfo() : m_keyCode(0), m_keyState(KeyState::JustReleased) {};
  BindInfo::BindInfo(unsigned int keyCode, const KeyState keyState) : m_keyCode(keyCode), m_keyState(keyState)
  {
  // Convert a small letter to a capital one
  if (97 <= keyCode && keyCode <= 122)
    keyCode -= 32;
  };

  //////////////////////////////////////////////////////////////////////////////
  // GameCommand Class /////////////////////////////////////////////////////////

  GameCommand::GameCommand() : m_chord(0) {};
  GameCommand::GameCommand(unsigned int keyCode, const KeyState keyState)
  {
  // Convert a small letter to a capital one
  if (97 <= keyCode && keyCode <= 122)
    keyCode -= 32;

  m_chord.push_back(BindInfo(keyCode, keyState));
  }
  GameCommand::GameCommand(const BindInfo& bi)
  {
  m_chord.push_back(bi);
  };
  GameCommand::GameCommand(const std::vector<BindInfo>& chord) : m_chord(chord) {};
}