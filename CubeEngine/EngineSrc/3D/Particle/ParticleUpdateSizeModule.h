#pragma once
#include "ParticleEmitterModule.h"
#include "Math/vec3.h"
namespace tzw {
struct Particle;
class ParticleUpdateSizeModule: public ParticleEmitterModule
{
public:
	virtual ~ParticleUpdateSizeModule() = default;
	ParticleUpdateSizeModule(float fromSize, float toSize);
	virtual void process(Particle * particle, ParticleEmitter * emitter);
private:
	float m_fromSize;
	float m_toSize;
};

} // namespace tzw