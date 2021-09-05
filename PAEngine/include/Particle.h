#pragma once
#include "..//..//PARender/inc/PARenderAPI/IModel.h"  //  InstanceID
#include <array>
#include <vector>

enum ModelType;
class ParticleBehavior;
class ParticleEmitter;
class Particle {
public:
  Particle(float ttl, std::array<float, 16> const& transform, ModelType mt, vec3 const& position);
  ~Particle();

  bool Alive() const;  //  true if there is still time to live
  float* GetTransform();  //  returns the transform used for drawing
  Rendering::IModel::InstanceID ID() const; //  retunrs the instance id
  void Update(float dt);  //  Update the particles ttl

  template <typename BEHAV_TYPE>
  BEHAV_TYPE* GetBehavior();
  
  void addBehavior(ParticleBehavior* behavior);

private:
  Rendering::IModel::InstanceID instance_;  //  InstanceID attached to this particle
  ModelType modelType_;
  std::array<float, 16> transform_;  //  current transform for this particle

  float ttl_;  //  time-to-live, time left until this particle is destroyed

  std::vector<ParticleBehavior*> particleBehaviors;

  friend class ParticleBehavior;
  friend class ParticleBehavColorChange;
};


template <typename BEHAV_TYPE>
BEHAV_TYPE* Particle::GetBehavior() {
  BEHAV_TYPE* behavior = nullptr;
  for (unsigned i = 0; i < particleBehaviors.size(); ++i) {
    behavior = dynamic_cast<BEHAV_TYPE*>(particleBehaviors[i]);
    if (behavior) {
        return behavior;
    }
  }
}
