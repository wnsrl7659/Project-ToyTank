#include "pch.h"
#include "AstarBehavStateDefault.h"
#include "AstarBehavior.h"

void AstarBehavStateDefault::Init()
{
}

void AstarBehavStateDefault::Update(float dt){
  astarBehavior_->astarColorUpdate();
  //astarBehavior_->groundUpdate();
  //astarBehavior_->Ending();
}

void AstarBehavStateDefault::Shutdown()
{
}
