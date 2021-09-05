/***************************************************************************//*
  file: SoundSystem.h
  author: j.mcdonell
  (c) DigiPen Institute of Technology
*//***************************************************************************/
#pragma once
#include <external/fmodstudio/fmod_studio.hpp>
#include <vector>
#include <array>

#define NUM_LOOPING_SOUNDS 8

/*  System used to control sounds. After initialization, sounds can be   *
 *  played by sending the the event ET_SOUND_PLAY with with data being   *
 *  an int for which sound to play           * 
 *  example: EventSystem::SendEvent(ET_SOUND_OS_PLAYERSHOT, nullptr, 0)  */
class SoundSystem {
  /*  this is a type of sound that we need to keep track of so we can  *
   *  control how its parameters are updated and when to end it.   */
  class LoopingSound {
  public: //  LoopingSound public functions
  LoopingSound();
  ~LoopingSound();
  void Create(FMOD::Studio::EventInstance* instance, float* parameter, unsigned index);
  void Clear();
  void Update();
  bool isValid();
  unsigned index_;  //  index of the description that created this
  private: // LoopingSound private members
  FMOD::Studio::EventInstance* instance_;
  float* parameter_;  
  };
public: //  SoundSystem public functions
  SoundSystem();
  ~SoundSystem();

  /*  Initialize the Sound System to make it ready to play sounds.  *
   *  Creates the FmodStudio systems, loads sound data from bank files  *
   *  and registers events with the EventSystem to control sounds   */
  void Init();

  /*  Updates the FmodStudio System, needs to be called every frame.  */
  void Update();

  /*  Cleans up the Fmod Studio system, unloads banks and sound data  */
  void Shutdown();

  /*  Gets the index into the eventDescription vector for the event   *
   *  with the given name             */
  unsigned GetSoundEventIndex(const char* name) const;

  /*  Creates an instance of the sound description at the given index   *
   *  Returns a pointer to that instance for setting parameters   */
  FMOD::Studio::EventInstance* PlayFmodSound(unsigned index);

  /*  Creates an instance of a looping sound, will have to be stopped later  */
  void StartLoopingSound(unsigned index, float* parameter = nullptr);

  /*  Stops a looping sound  */
  void EndLoopingSound(unsigned index);

  static SoundSystem* Get();

private: //  SoundSystem private members
  static SoundSystem* thisSystem_;  //  singleton pointer to this

  FMOD::Studio::System* system_;  //  fmods system 
  std::vector<FMOD::Studio::Bank*> banks_;  //  list of banks used to load sound events
  //  decription of sounds fmod uses to create instances of the sound
  std::vector<FMOD::Studio::EventDescription*> eventDescriptions_;  

  //  list of all the looping sounds so that we can control parameters and when to end
  std::array<LoopingSound, NUM_LOOPING_SOUNDS> loopingSounds_;

  FMOD_RESULT fmodErr_;  //  used for error checking with fmod functions

private: //  SoundSystem Private Functions   

  /*  finds the next empty spot in the looping sounds list  */
  unsigned FindEmpty();

};