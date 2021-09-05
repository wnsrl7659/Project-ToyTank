#pragma once
#include <random>
class RNG {
public:
  ~RNG();

  int RandInt(int lower, int upper);
  float RandFloat(float lower, float upper);
  bool CheckSuccess(float probaility);

  static RNG& GetRNG();
private:
  std::mt19937 rng_;

  RNG();
};

