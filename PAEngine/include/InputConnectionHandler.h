/**
 * @file InputConnectionHandler.h
 *
 * @brief It's an implementation of InputConnectionHandler.h
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

#include "InputDevices.h"

namespace PAEngine
{
  void DeviceConnected(Gamepad* const pGamepad);
  void DeviceDisconnected(Gamepad* const pGamepad);
}