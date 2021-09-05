#pragma once
#include "Behavior.h"
#include <array>

class ParticleEmitter;
class PlayerBehavior;
class FuelDepotBehavior : public Behavior {
public:
  enum {
    FDBS_INVALID = -1,
    FDBS_DEFAULT
  };

  FuelDepotBehavior();
  ~FuelDepotBehavior();

  void Create(int parentID, int fuelReserves, float playerFillSpeed, float reservesFillspeed);
  virtual void Destroy() override {};

  void StartFillingPlayer(int playerID);
  void StopFillingPlayer(int playerID);

private:
  int fuelReserves_;
  int maxFuel_;
  float fillSpeed_;
  float reservesFillSpeed_;
  float reserverFillTimer_;
  ParticleEmitter* pe_; // easy access to particle emitter
  std::array<bool, 4> playersInDepot_;

  bool CheckPlayerInRange(int playerID) const;
  bool PlayerInDepot(int playerID) const;
  void FillPlayer(int playerID);

  friend class FuelDepotBehavStateDefault;
};

