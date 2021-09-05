#include "pch.h"
#include "AIBehavStateDefault.h"
#include "AIBehavior.h"

void AIBehavStateDefault::Init()
{
}

void AIBehavStateDefault::Update(float dt)
{
  aiBehavior_->DefaultUpdate();
}

void AIBehavStateDefault::Shutdown()
{
}
