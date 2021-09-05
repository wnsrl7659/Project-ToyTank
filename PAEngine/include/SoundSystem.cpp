/******************************************************************************
  * Author : j.mcdonell
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#include "SoundSystem.h"
#include "ServiceLocator.h"
#include "EventSystem.h"
#include "SoundEventHandler.h"
#include <iostream>
#include <string.h>

#define MAX_CHANNELS 512
SoundSystem* SoundSystem::thisSystem_ = nullptr;

/*****************************************************************************/
/* typedefs for fmod studio objects                                          */
typedef FMOD::Studio::System* StudioSystemPtr;
typedef FMOD::Studio::Bank* BankPtr;
typedef FMOD::Studio::EventDescription* EventDescriptionPtr;
typedef FMOD::Studio::EventInstance* EventInstancePtr;

/*****************************************************************************/
/* forward declarations of static functions                                  */
static bool ErrCheck(FMOD_RESULT fmodErr_, const char* errMsg);
static void InitStudioSystem(StudioSystemPtr* system_, FMOD_RESULT& fmodErr_);
static void LoadBanks(StudioSystemPtr system_, std::vector<BankPtr>& banks_, FMOD_RESULT& fmodErr_);
static void LoadEventDescriptions(std::vector<BankPtr>& banks_, 
                                  std::vector<EventDescriptionPtr>& eventDescriptions_,
                                  FMOD_RESULT& fmodErr_);

/*****************************************************************************/
/* SoundSystem Member functions                                              */
SoundSystem::SoundSystem()
  : system_(nullptr), banks_(), fmodErr_(FMOD_OK) {
  if (thisSystem_ == nullptr) {
    thisSystem_ = this;
  }
}

SoundSystem::~SoundSystem(){
}


void SoundSystem::Init() {
  // initializing fmod studio and data loaded from files
  InitStudioSystem(&system_, fmodErr_);
  LoadBanks(system_, banks_, fmodErr_);
  LoadEventDescriptions(banks_, eventDescriptions_, fmodErr_);

  RegisterSoundEvents();
}

void SoundSystem::Update() {
  system_->update();

  //  updating all the looping sounds
  for (LoopingSound s : loopingSounds_) {
    s.Update();
  }
}

void SoundSystem::Shutdown() {
  system_->release();
}

unsigned SoundSystem::GetSoundEventIndex(const char* path) const {
  // create the buffer to retrieve the path from event descriptions
  size_t len = strlen(path) + 1;
  char* buffer = new char[len];
  //  find in description list using stsring compare
  for (unsigned i = 0; i < eventDescriptions_.size(); ++i) {
    eventDescriptions_[i]->getPath(buffer, len, nullptr);
    if (!strcmp(path, buffer)) {
      delete [] buffer;
      return i;
    }
  }
  delete [] buffer;
  return eventDescriptions_.size();
}

FMOD::Studio::EventInstance* SoundSystem::PlaySound(unsigned index) {
  EventInstancePtr inst = nullptr;
  // TODO: ERROR CHECKING!!!
  SoundSystem::thisSystem_->eventDescriptions_[index]->createInstance(&inst);
  inst->start();
  inst->release();
  return inst;
}

void SoundSystem::StartLoopingSound(unsigned index, float* parameter) {
  //  play the sound
  EventInstancePtr instance = PlaySound(index);
  //  add to the loop list
  unsigned EmptyLoopSound = FindEmpty();
  loopingSounds_[EmptyLoopSound].Create(instance, parameter, index);
}

void SoundSystem::EndLoopingSound(unsigned index) {
  //  find the first sound with this index
  for (unsigned i = 0; i < loopingSounds_.size(); ++i) {
    if (loopingSounds_[i].isValid() && loopingSounds_[i].index_ == index) {
      loopingSounds_[i].Clear(); // clear stops the sound and marks the spot empty
    }
  }
}

SoundSystem* SoundSystem::Get() {
  return thisSystem_;
}

unsigned SoundSystem::FindEmpty()
{
  for (unsigned i = 0; i < loopingSounds_.size(); ++i) {
    if (!loopingSounds_[i].isValid()) {
      return i;
    }
  }
  return loopingSounds_.size();
}

/*****************************************************************************/
/*  Looping Sound Member finctions                                           */
SoundSystem::LoopingSound::LoopingSound()
  : instance_(nullptr), parameter_(nullptr), index_(0) {
}

SoundSystem::LoopingSound::~LoopingSound(){
}

void SoundSystem::LoopingSound::Create(FMOD::Studio::EventInstance* instance, float* parameter, unsigned index) {
  instance_ = instance;
  parameter_ = parameter;
  index_ = index;
}

void SoundSystem::LoopingSound::Clear() {
  instance_->stop(FMOD_STUDIO_STOP_MODE::FMOD_STUDIO_STOP_ALLOWFADEOUT);
  instance_ = nullptr;
}

void SoundSystem::LoopingSound::Update() {
  if (parameter_) {
    EventDescriptionPtr desc = nullptr;
    FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
    instance_->getDescription(&desc);
    desc->getParameterDescriptionByIndex(0, &paramDesc);
    instance_->setParameterByID(paramDesc.id, *parameter_);
  }
}

bool SoundSystem::LoopingSound::isValid()
{
  if (instance_ == nullptr) {
    return false;
  }
  return true;
}

/*****************************************************************************/
/* Static functions                                                          */

/*  Checks the value of the error returned by an fmod function  *
 *  if there is an error it will print the errMsg to the log    *
 *  and return TRUE, otherwise, FALSE for no error.             */ 
static bool ErrCheck(FMOD_RESULT fmodErr_, const char* errMsg) {
  if (fmodErr_ != FMOD_OK) {
    Util::ServiceLocator::getFileLogger()->print(Util::error, errMsg);
    std::cout << fmodErr_;
    return true;
  }
  return false;
}

/*  InitStudioSystem, LoadBanks, and LoadEventDescriptions   *
 *  are used by Init to start the Fmod Studio System.        */
static void InitStudioSystem(StudioSystemPtr* system_, FMOD_RESULT& fmodErr_) {
  fmodErr_ = FMOD::Studio::System::create(system_);
  ErrCheck(fmodErr_, "FMOD error in Init, while creating studiosystem\n");

  fmodErr_ = (*system_)->initialize(MAX_CHANNELS, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
  ErrCheck(fmodErr_, "FMOD error in Init, while initalizing studiosystem\n");
}
static void LoadBanks(StudioSystemPtr system_, std::vector<BankPtr>& banks_, FMOD_RESULT& fmodErr_) {
  banks_.resize(2); // create pointers to fill in

  // loading banks, later make this generic? might not need because we dont have a sound designer.
  fmodErr_ = system_->loadBankFile("data/audio/banks/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &(banks_[0]));
  ErrCheck(fmodErr_, "FMOD error loading Master.bank\n");
  fmodErr_ = system_->loadBankFile("data/audio/banks/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &(banks_[1]));
  ErrCheck(fmodErr_, "FMOD error loading Master.strings.bank\n");
  fmodErr_ = system_->loadBankFile("data/audio/banks/Game.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &(banks_[1]));
  ErrCheck(fmodErr_, "FMOD error loading Master.strings.bank\n");
}
static void LoadEventDescriptions(std::vector<BankPtr>& banks_,
                                  std::vector<EventDescriptionPtr>& eventDescriptions_,
                                  FMOD_RESULT& fmodErr_) {
  // add up the event counts of each bank
  int totalEventCount = 0;
  int currentBankCount = 0;
  for (BankPtr b : banks_) {
    currentBankCount = 0;
    fmodErr_ = b->getEventCount(&currentBankCount);
    ErrCheck(fmodErr_, "Fmod Error counting events in the bank\n");
    totalEventCount += currentBankCount;
  }

  // we need to make sure there is enough room in the vecter 
  // to load all of the events
  eventDescriptions_.resize(totalEventCount);

  // loading the events from the banks into the description vector 
  int currentIdx = 0;
  for (BankPtr b : banks_) {
    currentBankCount = 0;
    if (currentIdx < eventDescriptions_.size()) {
      fmodErr_ = b->getEventList(&eventDescriptions_[currentIdx], totalEventCount - currentIdx, &currentBankCount);
      ErrCheck(fmodErr_, "Fmod Error loading event descriptions\n");
      currentIdx += currentBankCount;
    }
  }
}

