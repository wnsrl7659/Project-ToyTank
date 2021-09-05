#include "pch.h"
#include "PowerUpsBehavStateDefault.h"
#include "PowerUpsBehavior.h"

void PowerUpsBehavStateDefault::Init()
{
}

void PowerUpsBehavStateDefault::Update(float dt){
  PowerUpsBehavior_->PowerUpsUpdate();
}

void PowerUpsBehavStateDefault::Shutdown()
{
}
