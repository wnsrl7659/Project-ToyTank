#pragma once
#include "ComponentManager.h"

class ParticleManager;

class ParticleEmitterManager : public ComponentManager {
public:
  static ParticleEmitterManager* GetInstance();
  ~ParticleEmitterManager();

  void Init() override;  //  allocate mem for transform
  void Update(float dt) override;  //  update transform components
  void Draw() override;
  void Shutdown() override;  //  frees memory from transforms
private:
  ParticleEmitterManager();
  static ParticleEmitterManager* instance_;
  ParticleManager* pm_;
};

