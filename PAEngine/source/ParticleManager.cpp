#include "pch.h"
#include "ParticleManager.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "ParticleBehaviors.h"
#include "ModelComponent.h"
#include "ModelManager.h"
#include "..//..//PARender/inc/PARenderAPI/IModel.h" 

ParticleManager* ParticleManager::instance_ = nullptr;

ParticleManager::ParticleManager()
  : particles_(ParticleContainer()), particlesToDestroy_(std::stack<ParticleContainer::iterator>()) {
}

ParticleManager::~ParticleManager() {
  Shutdown();
  instance_ = nullptr;
}

void ParticleManager::Init() {
}

// update function for all particles
static void UpdateParticle(Particle& particle, float dt) {
  particle.Update(dt);
}

void ParticleManager::Update(float dt) {
  for (ParticleContainer::iterator particle = particles_.begin(); particle != particles_.end(); ++particle) {
    if (particle->second->Alive()) {  //  when the particle is alive it will move
      UpdateParticle(*(particle->second), dt);
    }
    else {
      // push the iterator on the stack of particles to delete this loop
      particlesToDestroy_.push(particle);
    }
  }
  // erase all the particles that should be destroyed
  ClearDeadParticles();
}

//  draws all the particles that are still alive
void ParticleManager::Draw() {
  for (ParticleContainer::iterator p = particles_.begin(); p != particles_.end(); ++p) {
    ModelManager::GetModel(p->first)->SetTransform(p->second->ID(), p->second->GetTransform());
  }
}

void ParticleManager::Shutdown() {
  //  put all the particles in the dead list and destroy them.
  for (ParticleContainer::iterator particle = particles_.begin(); particle != particles_.end(); ++particle) {
    particlesToDestroy_.push(particle);
  }
  ClearDeadParticles();
}

void ParticleManager::AddParticle(ModelType mt, float ttl, std::array<float, 16> const& transform, vec3 const& position) {
  Particle* p = new Particle(ttl, transform, mt, position);
  particles_.emplace(mt, p);
}

void ParticleManager::AddParticle(ParticleEmitter* emitter, vec3 const& position) {
  Particle* p = new Particle(emitter->ttl_, emitter->transform_, emitter->modelType_, position);
  if (emitter->particleTransform_.active) {
    std::array<float, 16>& transform = emitter->particleTransform_.transform_;
    float offsetx = RNG::GetRNG().RandFloat(-emitter->particleTransform_.offsetDirection, emitter->particleTransform_.offsetDirection);
    float offsety = RNG::GetRNG().RandFloat(-emitter->particleTransform_.offsetDirection, emitter->particleTransform_.offsetDirection);
    float offsetz = RNG::GetRNG().RandFloat(-emitter->particleTransform_.offsetDirection, emitter->particleTransform_.offsetDirection);

    transform[3] = emitter->particleTransform_.direction.x + offsetx;
    transform[7] = emitter->particleTransform_.direction.y + offsety;
    transform[11] = emitter->particleTransform_.direction.z + offsetz;

    ParticleBehavTransform* behavTransform = new ParticleBehavTransform(p, transform);
    p->addBehavior(behavTransform);
  }

  if (emitter->particleColor_.active) {
    ParticleBehavColorChange* colorBehav = new ParticleBehavColorChange(p);
    colorBehav->SetRGBA1(emitter->particleColor_.color1.x, emitter->particleColor_.color1.y, emitter->particleColor_.color1.z, emitter->particleColor_.a1);
    colorBehav->SetRGBA2(emitter->particleColor_.color2.x, emitter->particleColor_.color2.y, emitter->particleColor_.color2.z, emitter->particleColor_.a2);
    p->addBehavior(colorBehav);
  }
  particles_.emplace(emitter->modelType_, p);
}

ParticleManager* ParticleManager::Get()
{
  if (!instance_) {
    instance_ = new ParticleManager();
  }
  return instance_;
}

void ParticleManager::ClearDeadParticles() {
  while (!particlesToDestroy_.empty()) {
    ParticleContainer::iterator particle = particlesToDestroy_.top();
    ModelManager::GetModel(particle->first)->RemoveInstance(particle->second->ID());
    particles_.erase(particle);
    particlesToDestroy_.pop();
  }
}
