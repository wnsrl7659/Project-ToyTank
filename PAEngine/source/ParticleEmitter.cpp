#include "pch.h"
#include "ParticleEmitter.h"
#include "ModelComponent.h"
#include "ParticleManager.h"
#include "ObjectManager.h"
#include "ModelManager.h"

ParticleEmitter::ParticleEmitter(int managerIdx, ComponentManager* manager)
    : Component(managerIdx, manager), modelType_(MT_INVALID), transform_(), fireRate_(-1.f),
    timer_(0.f), ttl_(0.f), active_(false), burst_({ 0.f,0.f,0.f,0 }),
    particleTransform_({ {0}, vec3(), 0.f, false }),
    particleColor_({ vec3(), vec3(), 0.f, 0.f, false }),
    pm_(nullptr) {
}

ParticleEmitter::~ParticleEmitter() {
}

void ParticleEmitter::Create(int objectId, ModelType mt, float fireRate, float timer, float ttl, bool active) {
    modelType_ = mt;
    fireRate_ = fireRate;
    timer_ = timer;
    ttl_ = ttl;
    active_ = active;
    transform_ = { 1, 0, 0, 0,
               0, 1, 0, 0,
               0, 0, 1, 0,
               0, 0, 0, 1 };
    //ModelComponent* pModel = ObjectManager::GetObjectByID(objectId)->GetComponent<ModelComponent>();
    //if (pModel != nullptr)
    //{
    //  ModelManager::GetModel(pModel->GetModeltype())->SetColor(pModel->GetInstance(),
    //    1, 0, 0);
    //}
    pm_ = ParticleManager::Get();
    SetParent(objectId);
}

void ParticleEmitter::Destroy() {
    modelType_ = MT_INVALID;
    fireRate_ = -1.f;
    timer_ = -1.f;
    ttl_ = -1.f;
    active_ = false;
    particleTransform_.active = false;
    particleColor_.active = false;
    SetParent(-1);
}

void ParticleEmitter::BurstEmit(unsigned numParticles, float duration, float burstRate) {
    burst_.duration_ = duration;
    burst_.fireRate_ = burstRate;
    burst_.timeToNext_ = burstRate;
    burst_.numParticles_ = numParticles;
}

void ParticleEmitter::SetActive(bool active) {
    active_ = active;
}

void ParticleEmitter::SetTransformBehavior(vec3 const& direction, float angle, std::array<float, 16> const& transform) {
    particleTransform_.transform_ = transform;
    particleTransform_.direction = direction;
    particleTransform_.offsetDirection = angle;
    particleTransform_.active = true;
}

void ParticleEmitter::SetTransformBehavior(vec3 const& direction, float angle) {
    particleTransform_.direction = direction;
    particleTransform_.offsetDirection = angle;
}

void ParticleEmitter::SetColorBehavior(vec3 const& color1, vec3 const& color2, float a1, float a2) {
    particleColor_.color1 = color1;
    particleColor_.color2 = color2;
    particleColor_.a1 = a1;
    particleColor_.a2 = a2;
    particleColor_.active = true;
}

void ParticleEmitter::SetParticleTransform(std::array<float, 16>const& newParticleTransform) {
    transform_ = newParticleTransform;
}
