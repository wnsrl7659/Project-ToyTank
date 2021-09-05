#include "pch.h"
#include "Particle.h"
#include "ModelManager.h"
#include "ModelComponent.h"
#include "ParticleBehaviors.h"

Particle::Particle(float ttl, std::array<float, 16> const& transform, ModelType mt, vec3 const& position) 
  : instance_(), modelType_(mt), transform_(), ttl_(ttl) {

  // setup the particles transform
  transform_ = transform;
  // add the particles global position to the transform relative to the emitter
  transform_[12] += position.x;
  transform_[13] += position.y;
  transform_[14] += position.z;

  // create a new model instance for each particle
  instance_ = ModelManager::GetModel(mt)->AddInstance(nullptr);
  ModelManager::GetModel(mt)->Renderable(instance_, true);
}

Particle::~Particle() {
  for (unsigned i = 0; i < particleBehaviors.size(); ++i) {
    delete particleBehaviors[i];
  }
}

bool Particle::Alive() const {
  return ttl_ > 0;
}

float* Particle::GetTransform(){
  return transform_.data();
}

Rendering::IModel::InstanceID Particle::ID() const {
  return instance_;
}

void Particle::Update(float dt) {
  for (unsigned i = 0; i < particleBehaviors.size(); ++i) {
    particleBehaviors[i]->Update(dt);
  }
  ttl_ -= dt;
}

void Particle::addBehavior(ParticleBehavior* behavior) {
  particleBehaviors.push_back(behavior);
}
