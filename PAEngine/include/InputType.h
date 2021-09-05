/**
 * @file InputType.h
 *
 * @brief It's about a single key command and a serialized key command
 *
 * @author Minsu Kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

#include <vector>

namespace PAEngine
{
  //////////////////////////////////////////////////////////////////////////////
  // Input Detect Types ////////////////////////////////////////////////////////

  // enumeration of all possible key states
  enum KeyState { StillReleased, JustPressed, StillPressed, JustReleased };

  //////////////////////////////////////////////////////////////////////////////
  // BindInfo Class ////////////////////////////////////////////////////////////

  // structure to combine key codes and key states
  // each game command can have several bindings (chord)
  struct BindInfo
  {
  public:
  // constructors and destructor
  BindInfo();
  BindInfo(unsigned int keyCode, const KeyState keyState);
  ~BindInfo() {};

  // getters
  const unsigned int GetKeyCode() const { return m_keyCode; };

  friend struct GameCommand;
  friend class InputHandler;

  private:
  unsigned int m_keyCode;	// the actual key code
  KeyState m_keyState;	// the state the above specified key has to be in for the "binding" to become active
  };

  //////////////////////////////////////////////////////////////////////////////
  // GameCommand Class /////////////////////////////////////////////////////////

  // structure to map a single game command to a chord of key strokes (see above)
  struct GameCommand
  {
  public:
  // constructors and destructor
  GameCommand();
  GameCommand(unsigned int keyCode, const KeyState keyState);
  GameCommand(const BindInfo& bi);
  GameCommand(const std::vector<BindInfo>& chord);
  ~GameCommand() {};

  // get set chord
  const std::vector<BindInfo> GetChord() const { return m_chord; };
  void SetChord(std::vector<BindInfo> newChord) { m_chord = newChord; };

  friend class InputHandler;

  private:
  std::vector<BindInfo> m_chord;	// the chord mapped to this command, i.e. "shift"+"W" for Dash
  };
}