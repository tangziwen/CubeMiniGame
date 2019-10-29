#pragma once
#include "ParticleEmitterModule.h"
#include "Math/vec3.h"
namespace tzw {
struct Particle;
class ParticleInitAlphaModule: public ParticleEmitterModule
{
public:
	virtual ~ParticleInitAlphaModule() = default;
	ParticleInitAlphaModule(float lowerBound, float higherBound);
	virtual void process(Particle * particle);
private:
	float m_lowerBound;
	float m_higherBound;
};

} // namespace tzw