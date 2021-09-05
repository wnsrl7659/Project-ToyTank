/**
 * @file GameCommands.h
 *
 * @brief Commands in common use by every scene
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#pragma once

#include <vector>

namespace PAEngine
{
	//////////////////////////////////////////////////////////////////////////////
	// Command Functions /////////////////////////////////////////////////////////

	void ForwardFunction(void* data);
	void BackwardFunction(void* data);
	void LeftFunction(void* data);
	void BackwardFunction(void* data);
	void RightFunction(void* data);

	void DashFunction(void* data);

	void FireFunction(void* data);
  void PlayerBigShot(void* data);
  void BigShotPlaying(void* data);

  void PlayerBehaviorRapidFire(void* data);
  void PlayerBehaviorPlaying(void* data);

	void TestFunction(void* data);
  void KillFunction(void* data);
}