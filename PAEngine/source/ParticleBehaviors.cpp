#include "pch.h"
#include "ParticleBehaviors.h"
#include <DirectXMath.h>
#include "ModelManager.h"


ParticleBehavTransform::ParticleBehavTransform(Particle* particle, std::array<float, 16>const& transform)
  : ParticleBehavior(particle), transform_(transform) {
}

ParticleBehavTransform::~ParticleBehavTransform() {
}

void ParticleBehavTransform::Update(float dt) {
  float* particleTrans = particle_->GetTransform();
  DirectX::XMMATRIX matrix1(transform_.data());
  DirectX::XMMATRIX matrix2(particleTrans);
  DirectX::XMMATRIX multResult = DirectX::XMMatrixMultiply(DirectX::XMMatrixTranspose(matrix1), matrix2);
  for (unsigned i = 0; i < 16; ++i) {
    particleTrans[i] = multResult.r->m128_f32[i];
  }
}

ParticleBehavColorChange::ParticleBehavColorChange(Particle* particle)
  : ParticleBehavior(particle), rgb1_({ 0.f,0.f,0.f }), rgb2_({ 0.f,0.f,0.f }), 
    a1_(1.f), a2_(1.f),lifeSpan_(particle->ttl_) {
}

ParticleBehavColorChange::~ParticleBehavColorChange() {
}

void ParticleBehavColorChange::SetRGBA1(float r, float g, float b, float a) {
  SetRGB1(r, g, b);
  SetA1(a);
}

void ParticleBehavColorChange::SetRGB1(float r, float g, float b) {
  rgb1_ = vec3(r, g, b);
}

void ParticleBehavColorChange::SetA1(float a) {
  a1_ = a;
}

void ParticleBehavColorChange::SetRGBA2(float r, float g, float b, float a) {
  SetRGB2(r, g, b);
  SetA2(a);
}

void ParticleBehavColorChange::SetRGB2(float r, float g, float b) {
  rgb2_ = vec3(r, g, b);
}

void ParticleBehavColorChange::SetA2(float a) {
  a2_ = a;
}

void ParticleBehavColorChange::Update(float dt) {
  float interpolation = particle_->ttl_ / lifeSpan_;
  float newR = interpolation * rgb1_.x + (1 - interpolation) * rgb2_.x;
  float newG = interpolation * rgb1_.y + (1 - interpolation) * rgb2_.y;
  float newB = interpolation * rgb1_.z + (1 - interpolation) * rgb2_.z;
  float newA = interpolation * a1_ + (1/interpolation) * a2_;

  ModelManager::GetModel(particle_->modelType_)->SetColor(particle_->instance_, newR, newG, newB);
  ModelManager::GetModel(particle_->modelType_)->SetAlpha(particle_->instance_, newA);
}
