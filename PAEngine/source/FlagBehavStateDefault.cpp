#include "pch.h"
#include "FlagBehavStateDefault.h"
#include "FlagBehavior.h"

void FlagBehavStateDefault::Init()
{
}

void FlagBehavStateDefault::Update(float dt){
  //flagBehavior_->flagUpdate();
  flagBehavior_->ColorUpdate();
}

void FlagBehavStateDefault::Shutdown()
{
}
