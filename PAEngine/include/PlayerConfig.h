/**
 * @file PlayerBehaviorConfig.h
 *
 * @brief Configration for player's behavior
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

#define PLAYER_RESPAWN_TIME 5.f

// weapon sec/shot
#define PLAYER_WEAPONE_FIRERATE 1/5.0f
#define PLAYER_BIG_SHOT_FIRERATE 1/1.0f

// movement
#define PLAYER_MOVEMENT_SPEED 20.f // it's impulse which is related with the player tank's mass
#define PLAYER_MOVEMENT_MAX_SPEED 25.f
#define PLAYER_MOVEMENT_FRICTION 5.5f

// barrel (head)
#define PLAYER_BARREL_ROTATION_SPEED_HORIZONTAL 25.0f
#define PLAYER_BARREL_ROTATION_SPEED_VERTCIAL 7.5f
#define PLAYER_BARREL_YAXIS_MAX 0.3f 
#define PLAYER_BARREL_YAXIS_MIN 0.03f // the reason for its low value is to prevent a bouncing bullet problem

// body
#define PLAYER_BODY_ROTATION_SPEED 1.5f