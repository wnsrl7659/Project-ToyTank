/******************************************************************************
  file: SoundEventHandler.cpp
  author: Joseph McDonell

  (c) DigiPen Institute of Technology

  This file contains functions to handle sound events.
  These functions handle playing one-shot sounds, starting
  and stopping looping sounds.
  The data passed in to thes functions should be a pointer
  to the parameter that will tell the sound how to play.

  Example: 
  To start the sound of the player moving, we need the
  players speed as a parameter, so we send this event:
  EventSystem::SendEvent(ET_SOUND_L_START_PLAYERMOVE,
         (void*)&playerSpeed, sizeof(float*))
******************************************************************************/
#include "pch.h"
#include "SoundEventHandler.h"
#include "SoundSystem.h"
#include "EventSystem.h"

/*  functions registered with the event system  */
static void PlayerShootSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Game/PlayerShoot");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void PlayerRapidShootSound(void* data) {
  static unsigned index =
    SoundSystem::Get()->GetSoundEventIndex("event:/Game/RapidFire");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void ShotHitSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Game/ShotHit");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void DebrisSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Game/Debris");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void FlagPickupSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Game/FlagPickup");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void FlagDropSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Game/FlagDrop");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void PlayerWinSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Game/PlayerWin");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void ButtonSelectSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Menu/ButtonSelect");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void ButtonNavigationSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Menu/ButtonNavigate");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void ExplosionSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Game/Explosion");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void FeulFullSound(void* data) {
  static unsigned index =
    SoundSystem::Get()->GetSoundEventIndex("event:/Game/FeulFull");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void FeulemptySound(void* data) {
  static unsigned index =
    SoundSystem::Get()->GetSoundEventIndex("event:/Game/FuelEmpty");
  SoundSystem::Get()->PlayFmodSound(index);
}
static void SrartRefuelSound(void* data) {
  static unsigned index =
    SoundSystem::Get()->GetSoundEventIndex("event:/Game/Refuel");
  FMOD::Studio::EventInstance* inst = SoundSystem::Get()->PlayFmodSound(index);
  
  // set the parameter on the instance
  inst->setParameterByName("feul", *(float*)data);
}
static void PaintSound(void* data) {
  static unsigned index =
    SoundSystem::Get()->GetSoundEventIndex("event:/Game/Paint");
  FMOD::Studio::EventInstance* inst = SoundSystem::Get()->PlayFmodSound(index);
}

static void StartScoreCounterSound(void* data) {
  static unsigned index =
    SoundSystem::Get()->GetSoundEventIndex("event:/Game/ScoreCounter");
  SoundSystem::Get()->StartLoopingSound(index, reinterpret_cast<float*>(data));
}
static void EndScoreCounterSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Game/ScoreCounter");
  SoundSystem::Get()->EndLoopingSound(index);
}
static void StartPlayerMoveSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Game/PlayerMove");
  SoundSystem::Get()->StartLoopingSound(index, reinterpret_cast<float*>(data));
}
static void EndPlayerMoveSound(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Game/PlayerMove");
  SoundSystem::Get()->EndLoopingSound(index);
}
static void StartGameMusic(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Music/GameMusic");
  SoundSystem::Get()->StartLoopingSound(index, reinterpret_cast<float*>(data));
}
static void EndGameMusic(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Music/GameMusic");
  SoundSystem::Get()->EndLoopingSound(index);
}
static void StartMenuMusic(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Music/MenuMusic");
  SoundSystem::Get()->StartLoopingSound(index, reinterpret_cast<float*>(data));
}
static void EndMenuMusic(void* data) {
  static unsigned index =
  SoundSystem::Get()->GetSoundEventIndex("event:/Music/MenuMusic");
  SoundSystem::Get()->EndLoopingSound(index);
}

void RegisterSoundEvents() {
  EventSystem::RegisterEvent(ET_SOUND_OS_PLAYERSHOOT, PlayerShootSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_SHOTHIT, ShotHitSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_DEBRIS, DebrisSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_TURRETCAPTURE, FlagPickupSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_FLAGDROP, FlagDropSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_PLAYERWIN, PlayerWinSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_BUTTONSELECT, ButtonSelectSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_BUTTONNAVIGATION, ButtonNavigationSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_EXPLOSION, ExplosionSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_FUELEMPTY, FeulemptySound);
  EventSystem::RegisterEvent(ET_SOUND_OS_FUELFULL, FeulFullSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_REFUEL, SrartRefuelSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_PAINT, PaintSound);
  EventSystem::RegisterEvent(ET_SOUND_OS_RAPIDFIRE, PlayerRapidShootSound);
  EventSystem::RegisterEvent(ET_SOUND_L_START_SCORECOUNTER, StartScoreCounterSound);
  EventSystem::RegisterEvent(ET_SOUND_L_END_SCORECOUNTER, EndScoreCounterSound);
  EventSystem::RegisterEvent(ET_SOUND_L_START_PLAYERMOVE, StartPlayerMoveSound);
  EventSystem::RegisterEvent(ET_SOUND_L_END_PLAYERMOVE, EndPlayerMoveSound);
  EventSystem::RegisterEvent(ET_SOUND_L_START_GAMEMUSIC, StartGameMusic);
  EventSystem::RegisterEvent(ET_SOUND_L_END_GAMEMUSIC, EndGameMusic);
  EventSystem::RegisterEvent(ET_SOUND_L_START_MENUMUSIC, StartMenuMusic);
  EventSystem::RegisterEvent(ET_SOUND_L_END_MENUMUSIC, EndMenuMusic);
}
