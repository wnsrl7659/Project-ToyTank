#pragma once
#include <map>
#include <stack>

enum ModelType;
class Particle;
class ModelManager;
class ParticleEmitter;

typedef std::multimap<ModelType, Particle*> ParticleContainer;

class ParticleManager {
public:
  ParticleManager();
  ~ParticleManager();

  void Init();
  void Update(float dt);
  void Draw();
  void Shutdown();

  void AddParticle(ModelType mt, float ttl, std::array<float, 16> const& transform, vec3 const& position);  //  adds a particle to the map
  void AddParticle(ParticleEmitter* emitter, vec3 const& position);  //  adds a particle to the map

  static ParticleManager* Get();

private:
  static ParticleManager* instance_;
  ParticleContainer particles_;
  std::stack<ParticleContainer::iterator> particlesToDestroy_;

  void ClearDeadParticles();
};

