#pragma once
#include "ParticleEmitterModule.h"
#include "Math/vec3.h"
namespace tzw {
struct Particle;
class ParticleUpdateAlphaModule: public ParticleEmitterModule
{
public:
	virtual ~ParticleUpdateAlphaModule() = default;
	ParticleUpdateAlphaModule(float alphaSpeed);
	virtual void process(Particle * particle, ParticleEmitter * emitter);
private:
	float m_alphaSpeed;
	vec3 m_higherBound;
};

} // namespace tzw