#include "pch.h"
#include "GaugeBehavStateDefault.h"
#include "GaugeBehavior.h"

void GaugeBehavStateDefault::Init()
{
}

void GaugeBehavStateDefault::Update(float dt)
{
  gaugeBehavior_->GaugeUpdate();
}

void GaugeBehavStateDefault::Shutdown()
{
}
