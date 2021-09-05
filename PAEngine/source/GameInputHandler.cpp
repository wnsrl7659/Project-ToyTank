/**
 * @file GameInputDevices.cpp
 *
 * @brief This Handler is for Game Playing which needs commend setting
 *
 * @author Minsu kang
 *
 * @copyright (c) DigiPen Institute of Technology
 */

#include "pch.h"
#include "GameInputHandler.h"
#include "GameCommands.h"

namespace PAEngine
{
  GameInput::GameInput()
  {
    ClearKeyMapKBM();
    ClearKeyMapGamepadInCommon();

    SetDefaultKeyMap();
  }

  void GameInput::SetDefaultKeyMap()
  {
    /////////////////////////////////////
    // two-key combination command //////
    // It has critical bug yet

//#ifdef _DEBUG
    ///////////////////////////////////////////////////////////////////////
    // keyboard and mouse input ///////////////////////////////////////////

    // set key combination (LShift + W = dash)
    std::vector<BindInfo> biKBM;
    biKBM.push_back(BindInfo(VK_LSHIFT, KeyState::StillPressed));
    biKBM.push_back(BindInfo('W', KeyState::StillPressed));

    SetGameInputKBM(EventType::ET_INPUT_DASH,
      new GameCommand(biKBM),
      DashFunction);

    /////////////////////////////////////
    // one key command //////////////////

    // W = forward
    SetGameInputKBM(EventType::ET_INPUT_FORWARD,
      new GameCommand('W', KeyState::StillPressed),
      ForwardFunction);

    // S = backward
    SetGameInputKBM(EventType::ET_INPUT_BACKWARD,
      new GameCommand('S', KeyState::StillPressed),
      BackwardFunction);

    // A = left
    SetGameInputKBM(EventType::ET_INPUT_LEFT,
      new GameCommand('A', KeyState::StillPressed),
      LeftFunction);

    // D = right
    SetGameInputKBM(EventType::ET_INPUT_RIGHT,
      new GameCommand('D', KeyState::StillPressed),
      RightFunction);

    // X = just x
    SetGameInputKBM(EventType::ET_INPUT_X,
      new GameCommand('X', KeyState::JustPressed),
      nullptr);

    SetGameInputKBM(EventType::ET_INPUT_ML,
      new GameCommand('A', KeyState::JustPressed),
      nullptr);

    SetGameInputKBM(EventType::ET_INPUT_MR,
      new GameCommand('D', KeyState::JustPressed),
      nullptr);

    SetGameInputKBM(EventType::ET_INPUT_MU,
      new GameCommand('W', KeyState::JustPressed),
      nullptr);

    SetGameInputKBM(EventType::ET_INPUT_MD,
      new GameCommand('S', KeyState::JustPressed),
      nullptr);

    SetGameInputKBM(EventType::ET_INPUT_A,
      new GameCommand(VK_RETURN, KeyState::JustPressed),
      nullptr);


    /////////////////////////////////////
    // virtual key command //////////////

    SetGameInputKBM(EventType::ET_INPUT_LOOK_UP_StillPressed,
      new GameCommand(VK_UP, KeyState::StillPressed));

    SetGameInputKBM(EventType::ET_INPUT_LOOK_UP_JustReleased,
      new GameCommand(VK_UP, KeyState::JustReleased));

    SetGameInputKBM(EventType::ET_INPUT_LOOK_DOWN_StillPressed,
      new GameCommand(VK_DOWN, KeyState::StillPressed));

    SetGameInputKBM(EventType::ET_INPUT_LOOK_DOWN_JustReleased,
      new GameCommand(VK_DOWN, KeyState::JustReleased));

    SetGameInputKBM(EventType::ET_INPUT_LOOK_LEFT_StillPressed,
      new GameCommand(VK_LEFT, KeyState::StillPressed));

    SetGameInputKBM(EventType::ET_INPUT_LOOK_LEFT_JustReleased,
      new GameCommand(VK_LEFT, KeyState::JustReleased));

    SetGameInputKBM(EventType::ET_INPUT_LOOK_RIGHT_StillPressed,
      new GameCommand(VK_RIGHT, KeyState::StillPressed));

    SetGameInputKBM(EventType::ET_INPUT_LOOK_RIGHT_JustReleased,
      new GameCommand(VK_RIGHT, KeyState::JustReleased));

    // ESC = quit
    // if the actin is very simple, it's okay to not have a function
    SetGameInputKBM(EventType::ET_INPUT_QUIT,
      new GameCommand(VK_ESCAPE,
        KeyState::JustPressed));

    // Right Shift - fire on
    SetGameInputKBM(EventType::ET_INPUT_BEHAVIOR_RAPID_FIRE,
      new GameCommand(VK_RSHIFT, KeyState::JustPressed),
      PlayerBehaviorRapidFire);

    // Right Shift - fire off
    SetGameInputKBM(EventType::ET_INPUT_BEHAVIOR_PLATING,
      new GameCommand(VK_RSHIFT, KeyState::JustReleased),
      PlayerBehaviorPlaying);

    // left Trigger - fire on
    SetGameInputKBM(EventType::ET_INPUT_FIRE,
      new GameCommand('E', KeyState::JustPressed),
      PlayerBigShot);

    // left Trigger - fire off
    SetGameInputKBM(EventType::ET_INPUT_BEHAVIOR_PLATING,
      new GameCommand('E', KeyState::JustReleased),
      BigShotPlaying);

    // SpaceBer = test
    SetGameInputKBM(EventType::ET_INPUT_TEST,
      new GameCommand(VK_SPACE, KeyState::JustPressed),
      TestFunction);

    // SpaceBer = test
    SetGameInputKBM(EventType::ET_INPUT_KILL,
      new GameCommand('K', KeyState::JustPressed),
      KillFunction);
//#endif

    ///////////////////////////////////////////////////////////////////////
    // XInput /////////////////////////////////////////////////////////////

      // set key combination (LShift + W = dash)
    std::vector<BindInfo> biXInput;
    biXInput.push_back(BindInfo(VK_PAD_LTRIGGER, KeyState::StillPressed));
    biXInput.push_back(BindInfo(XINPUT_GAMEPAD_LEFT_SHOULDER, KeyState::StillPressed));

    // set the key combination to the keyboard input (LShift + W = dash)
    SetGameInput(EventType::ET_INPUT_DASH,
      new GameCommand(biXInput),
      DashFunction);

    // DPAD_UP = forward
    SetGameInput(EventType::ET_INPUT_FORWARD,
      new GameCommand(XINPUT_GAMEPAD_DPAD_UP, KeyState::StillPressed),
      ForwardFunction);

    // DPAD_DOWN = backward
    SetGameInput(EventType::ET_INPUT_BACKWARD,
      new GameCommand(XINPUT_GAMEPAD_DPAD_DOWN, KeyState::StillPressed),
      BackwardFunction);

    // DPAD_LEFT = left
    SetGameInput(EventType::ET_INPUT_LEFT,
      new GameCommand(XINPUT_GAMEPAD_DPAD_LEFT, KeyState::StillPressed),
      LeftFunction);

    // DPAD_RIGHT = right
    SetGameInput(EventType::ET_INPUT_RIGHT,
      new GameCommand(XINPUT_GAMEPAD_DPAD_RIGHT, KeyState::StillPressed),
      RightFunction);


    // Right Trigger - fire on
    SetGameInput(EventType::ET_INPUT_BEHAVIOR_RAPID_FIRE,
      new GameCommand(VK_PAD_RTRIGGER, KeyState::JustPressed),
      PlayerBehaviorRapidFire);

    // Right Trigger - fire off
    SetGameInput(EventType::ET_INPUT_BEHAVIOR_PLATING,
      new GameCommand(VK_PAD_RTRIGGER, KeyState::JustReleased),
      PlayerBehaviorPlaying);

    // left Trigger - fire on
    SetGameInput(EventType::ET_INPUT_FIRE,
      new GameCommand(VK_PAD_LTRIGGER, KeyState::JustPressed),
      PlayerBigShot);

    // left Trigger - fire off
    SetGameInput(EventType::ET_INPUT_BEHAVIOR_PLATING,
      new GameCommand(VK_PAD_LTRIGGER, KeyState::JustReleased),
      BigShotPlaying);


    // Back or ESC = quit
    // if the actin is very simple, it's okay to not have a function
    SetGameInput(EventType::ET_INPUT_QUIT,
      new GameCommand(XINPUT_GAMEPAD_BACK, KeyState::JustPressed));

    SetGameInput(EventType::ET_INPUT_START,
      new GameCommand(XINPUT_GAMEPAD_START, KeyState::JustPressed));


    SetGameInput(EventType::ET_INPUT_ML,
      new GameCommand(XINPUT_GAMEPAD_DPAD_LEFT, KeyState::JustPressed),
      nullptr);

    SetGameInput(EventType::ET_INPUT_MR,
      new GameCommand(XINPUT_GAMEPAD_DPAD_RIGHT, KeyState::JustPressed),
      nullptr);

    SetGameInput(EventType::ET_INPUT_MU,
      new GameCommand(XINPUT_GAMEPAD_DPAD_UP, KeyState::JustPressed),
      nullptr);

    SetGameInput(EventType::ET_INPUT_MD,
      new GameCommand(XINPUT_GAMEPAD_DPAD_DOWN, KeyState::JustPressed),
      nullptr);
    SetGameInput(EventType::ET_INPUT_A,
      new GameCommand(XINPUT_GAMEPAD_A, KeyState::JustPressed),
      nullptr);



    // set the key layout to each available player
    std::vector<Gamepad*>::iterator it = GetGamePads().begin();
    while (it != GetGamePads().end())
    {
      SetKeyMapToPlayer((*it)->GetPlayerID());

      ++it;
    }
  }

  const bool GameInput::SetGameInput(EventType _eventType, GameCommand* _pGameCommand, EventHandler function)
  {
    gamepadKeyMapInCommon.insert(TypeCommandPair(_eventType, _pGameCommand));

    if (function != nullptr)
      EventSystem::RegisterEvent(_eventType, function);

    return true;
  }

  const bool GameInput::SetGameInputKBM(EventType _eventType, GameCommand* _pGameCommand, EventHandler function)
  {
    GetKeyboardAndMouse()->pKeyMap->insert(TypeCommandPair(_eventType, _pGameCommand));

    if (function != nullptr)
      EventSystem::RegisterEvent(_eventType, function);

    return true;
  }
}