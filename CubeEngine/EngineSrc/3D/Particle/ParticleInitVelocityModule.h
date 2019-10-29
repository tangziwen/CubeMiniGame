#pragma once
#include "ParticleEmitterModule.h"
#include "Math/vec3.h"
namespace tzw {
struct Particle;
class ParticleInitVelocityModule: public ParticleEmitterModule
{
public:
	virtual ~ParticleInitVelocityModule() = default;
	ParticleInitVelocityModule(vec3 lowerBound, vec3 higherBound);
	virtual void process(Particle * particle);
private:
	vec3 m_lowerBound;
	vec3 m_higherBound;
};

} // namespace tzw