/**
 * @file PlayerAttributesController.h
 *
 * @brief It controlls player's attributes for other members:
 *        Firerate, move velocity, and etc.
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

class PlayerAttController
{
public:
  static void ResetAllAttributes(const int playerID);

  static void SetHP(const int playerID, const int hp);
  static void SetFireRate(const int playerID, const float fireRate);
  static void SetMoveVelocity(const int playerID, const float moveVelocity);
  static void SetMoveMaxVelocity(const int playerID, const float maxVelocity);
  static void SetTwinkling(const int playerID, const int isTwinkling);
  static void SetColor(const int playerID, const vec3 color);

  static const float GetFireRate(const int playerID);
  static const float GetMoveVelocity(const int playerID);
  static const float GetMoveMaxVelocity(const int playerID);
};