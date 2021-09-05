#pragma once
#include "ModelComponent.h"
#include "Component.h"
#include <array>
class ParticleManager;

class ParticleEmitter : public Component {
public:
  ParticleEmitter(int managerIdx, ComponentManager* manager);
  ~ParticleEmitter(); 

  void Create(int objectId, ModelType mt = MT_INVALID, float fireRate = -1.f, float timer = -1.f, float ttl = -1.f, bool active = false);
  void Destroy() override;

  /* sets data for burst particlee emittion                         *
   * numParticles : number of particles per burst                   *
   * duaration : how long (seconds) the burst emittion will last    *
   *             default 0 (1 burst)
   * burstRate : time between bursts (default 0, burst every frame) */
  void BurstEmit(unsigned numParticles, float duration = 0, float burstRate = 0);
  
  /* turns particle emitter on or off *
   * true = on                        *
   * false = off                      */
  void SetActive(bool active);

  void SetTransformBehavior(vec3 const& direction, float angle, std::array<float, 16>const& transform );
  void SetTransformBehavior(vec3 const& direction, float angle);
  void SetColorBehavior(vec3 const& color1, vec3 const& color2, float a1, float a2);

  void SetParticleTransform(std::array<float, 16>const& newParticleTransform);

private:
  ModelType modelType_;  //  detirmins what the particles look like
  std::array<float, 16> transform_;  //  starting transform for particles
  float fireRate_;  //  how often particles are fired
  float timer_;  //  time until next particle is fired
  float ttl_;   //  ttl for each particle
  bool active_;  //  emitter will only create particles if it is active
  
  // data used for patricle bursts, no burst if numParticles == 0
  struct BurstData {
    float fireRate_; // max firerate of the burst
    float timeToNext_; // time until next burst is fired
    float duration_; // total duration of burst emittion
    unsigned numParticles_; // number of particles to fire each burst
  }burst_;

  // data for particleBehavTransform
  struct TransformData {
    std::array<float, 16> transform_; // base transformation the particle will go through each frame
    vec3 direction; // direction the particle will move in
    float offsetDirection; // max offset away from direction the particle will move
    bool active;
  } particleTransform_;

  struct ColorData {
    vec3 color1;
    vec3 color2;
    float a1;
    float a2;
    bool active;
  } particleColor_;

  ParticleManager* pm_;

  friend class ParticleEmitterManager;
  friend class ParticleManager;
};

