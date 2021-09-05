#include "pch.h"
#include "RNG.h"

RNG::RNG() 
  : rng_() { 
  std::random_device dev;
  rng_.seed(dev());
}

RNG::~RNG(){
}

int RNG::RandInt(int lower, int upper) {
  std::uniform_int_distribution<> dist(lower, upper - 1);
  return dist(rng_);
}

float RNG::RandFloat(float lower, float upper)
{
  std::uniform_real_distribution<> dist(lower, upper);
  float result = dist(rng_);
  return result;
}

bool RNG::CheckSuccess(float probaility) {
  if (probaility >= 0.f && probaility <= 1.f) {
    return probaility >= RandFloat(0.f, 1.f);
  }
  return false;
}

RNG& RNG::GetRNG() {
  static RNG instance;
  return instance;
}
