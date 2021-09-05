#include "pch.h"
#include "ParticleEmitterManager.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "ObjectManager.h"
#include "ChildTransform.h"

ParticleEmitterManager* ParticleEmitterManager::instance_ = nullptr;

ParticleEmitterManager::ParticleEmitterManager() : ComponentManager() {
}

ParticleEmitterManager* ParticleEmitterManager::GetInstance() {
  if (!instance_) {
    instance_ = new ParticleEmitterManager();
  }
  return instance_;
}

ParticleEmitterManager::~ParticleEmitterManager(){
}

void ParticleEmitterManager::Init(){
  //  allocate memory for ChildTransforms on base classes array
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    components_[i] = new ParticleEmitter(i, this);
  }
  // itnitialize the particle manager
  pm_ = ParticleManager::Get();
}

void ParticleEmitterManager::Update(float dt) {
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    if (allocated_[i]) {
      ParticleEmitter* emitter = reinterpret_cast<ParticleEmitter*>(components_[i]);
      // update regular particle emittion
      if (emitter->active_)
      {
        if (emitter->timer_ < 0) {
          Transform* objecTransform = ObjectManager::GetObjectByID(emitter->GetParent())->GetComponent<Transform>();
          vec3 particlePosition = vec3(objecTransform->GetPositionX(), objecTransform->GetPositionY(), objecTransform->GetPositionZ());
          
          pm_->AddParticle(emitter, particlePosition );
          emitter->timer_ = emitter->fireRate_;
        }
        else {
          emitter->timer_ -= dt;
        }
      }

      // update burst particle emittion
      if (emitter->burst_.numParticles_ > 0) {
        if (emitter->burst_.duration_ >= 0) {
          emitter->burst_.duration_ -= dt;
          emitter->burst_.timeToNext_ -= dt;
          if (emitter->burst_.timeToNext_ <= 0) {
            Transform* objecTransform = ObjectManager::GetObjectByID(emitter->GetParent())->GetComponent<Transform>();
            vec3 particlePosition = vec3(objecTransform->GetPositionX(), objecTransform->GetPositionY(), objecTransform->GetPositionZ());

            for (unsigned i = 0; i < emitter->burst_.numParticles_; ++i) {
              pm_->AddParticle(emitter, particlePosition);
            }
            emitter->burst_.timeToNext_ = emitter->burst_.fireRate_;
          }
        }
        else {
          // done burst emitting
          emitter->burst_.numParticles_ = 0;
        }
      }

    }
  }

  pm_->Update(dt);
}

void ParticleEmitterManager::Draw() {
  pm_->Draw();
}

void ParticleEmitterManager::Shutdown() {
  //  delete memory allocated in init
  for (unsigned i = 0; i < OM_MAX_OBJECTS; ++i) {
    delete components_[i];
    components_[i] = nullptr;
    allocated_[i] = false;
  }
}
