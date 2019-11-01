#pragma once
#include "ParticleEmitterModule.h"
#include "Math/vec3.h"
namespace tzw {
struct Particle;
class ParticleInitPosModule: public ParticleEmitterModule
{
public:
	virtual ~ParticleInitPosModule() = default;
	ParticleInitPosModule(vec3 lowerBound, vec3 higherBound);
	void process(Particle * particle, ParticleEmitter * emitter) override;
private:
	vec3 m_lowerBound;
	vec3 m_higherBound;
};

} // namespace tzw