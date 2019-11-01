#pragma once
#include "ParticleEmitterModule.h"
#include "Math/vec3.h"
namespace tzw {
struct Particle;
class ParticleInitSizeModule: public ParticleEmitterModule
{
public:
	virtual ~ParticleInitSizeModule() = default;
	ParticleInitSizeModule(float lowerBound, float higherBound);
	virtual void process(Particle * particle, ParticleEmitter * emitter);
private:
	float m_lowerBound;
	float m_higherBound;
};

} // namespace tzw