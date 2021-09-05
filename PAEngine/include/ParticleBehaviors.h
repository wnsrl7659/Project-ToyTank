#pragma once
#include <array>
#include "Particle.h"

class ParticleBehavior {
public:
  ParticleBehavior(Particle* particle) : particle_(particle){};
  ~ParticleBehavior() {};

  virtual void Update(float dt) = 0;
protected:
  Particle* particle_;
  float* particleTransform() { return particle_->transform_.data(); }
};

// transformation done on the particle
class ParticleBehavTransform : public ParticleBehavior {
public:
  ParticleBehavTransform(Particle* particle, std::array<float, 16>const & transform);
  ~ParticleBehavTransform();

  void Update(float dt) override;
private:
  std::array<float, 16> transform_;
};

// interpolate between two colors over the particles lifespan
class ParticleBehavColorChange : public ParticleBehavior {
public:
  ParticleBehavColorChange(Particle* particle);
  ~ParticleBehavColorChange();

  void SetRGBA1(float r, float g, float b, float a);
  void SetRGB1 (float r, float g, float b);
  void SetA1   (float a);
  void SetRGBA2(float r, float g, float b, float a);
  void SetRGB2 (float r, float g, float b);
  void SetA2   (float a);

  void Update(float dt) override;
private:
  vec3 rgb1_, rgb2_; // starting and ending colors
  float a1_, a2_; // starting and ending alpha values
  float lifeSpan_; // total lifespan of the particle
};