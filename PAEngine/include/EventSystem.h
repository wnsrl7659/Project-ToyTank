/******************************************************************************
  file: EventSystem.h
  author: j.mcdonell

  (c) DigiPen Institute of Technology
******************************************************************************/
#pragma once
/* Forward Declarations */
class EventRegistry;
class EventSender;

/* These are the event types to be registered */
enum EventType {
  ET_INVALID = -1,
  ET_TEST = 0,
  ET_RENDER,
  ET_GUIONOFF,
  ET_MENUONOFF,
  ET_WINONOFF,
  ET_UIONOFF,
  ET_PAUSEONOFF,
  ET_LOGOONOFF,
  ET_PRESSONOFF,
  ET_FINISHONOFF,
  ET_CONFIRMONOFF,
  ET_CONNECTONOFF,

  // input commands
  ET_INPUT_ML,
  ET_INPUT_MR,
  ET_INPUT_MU,
  ET_INPUT_MD,
  ET_INPUT_A,
  ET_INPUT_B,
  ET_INPUT_X,
  ET_INPUT_QUIT,
  ET_INPUT_START,

  // One-Shot Sound effects
  ET_SOUND_OS_PLAYERSHOOT,
  ET_SOUND_OS_RAPIDFIRE,
  ET_SOUND_OS_SHOTHIT,
  ET_SOUND_OS_DEBRIS,
  ET_SOUND_OS_TURRETCAPTURE,
  ET_SOUND_OS_FLAGDROP,
  ET_SOUND_OS_PLAYERWIN,
  ET_SOUND_OS_EXPLOSION,
  ET_SOUND_OS_FUELEMPTY,
  ET_SOUND_OS_FUELFULL,
  ET_SOUND_OS_REFUEL,
  ET_SOUND_OS_PAINT,
  ET_SOUND_OS_BUTTONNAVIGATION,
  ET_SOUND_OS_BUTTONSELECT,
  // Looping Sound Effects. They need to be told when to start and end
  ET_SOUND_L_START_SCORECOUNTER,
  ET_SOUND_L_END_SCORECOUNTER,
  ET_SOUND_L_START_PLAYERMOVE,
  ET_SOUND_L_END_PLAYERMOVE,
  ET_SOUND_L_START_GAMEMUSIC,
  ET_SOUND_L_END_GAMEMUSIC,
  ET_SOUND_L_START_MENUMUSIC,
  ET_SOUND_L_END_MENUMUSIC,

  // start - ignore these types when in paused state - start
  ET_INPUT_LOOK_UP_StillPressed,
  ET_INPUT_LOOK_UP_JustReleased,
  ET_INPUT_LOOK_DOWN_StillPressed,
  ET_INPUT_LOOK_DOWN_JustReleased,
  ET_INPUT_LOOK_LEFT_StillPressed,
  ET_INPUT_LOOK_LEFT_JustReleased,
  ET_INPUT_LOOK_RIGHT_StillPressed,
  ET_INPUT_LOOK_RIGHT_JustReleased,
  ET_INPUT_FORWARD,
  ET_INPUT_BACKWARD,
  ET_INPUT_LEFT,
  ET_INPUT_RIGHT,
  ET_INPUT_DASH,
  ET_INPUT_FIRE,
  ET_INPUT_BEHAVIOR_RAPID_FIRE,
  ET_INPUT_BEHAVIOR_PLATING,
  ET_INPUT_KEY_CHANGING,
  ET_INPUT_TEST,
  ET_INPUT_KILL,


  // end - ignore these types when in paused state - end

  ET_REGISTRY_SIZE
};

/* function should be defined for each event type */
typedef void (*EventHandler)(void*);

/* singleton class used to Register and sendn events from other systems. 
   This is created when the EventProcessor is created       */
class EventSystem {
public:
  /* Sends an event to be processed later */
  static void SendEvent(EventType type, void* data, unsigned dataSize);

  /* Regester functions to handle Events */
  static void RegisterEvent(EventType type, EventHandler function);

private:
  friend class EventProcessor;
  EventSystem(EventRegistry* registry, EventSender* eventQueue);
  ~EventSystem();

  EventRegistry* registry_;
  EventSender* eventQueue_;

  static EventSystem* thisSystem_;
};