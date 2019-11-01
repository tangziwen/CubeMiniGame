#pragma once
#include "ParticleEmitterModule.h"
#include "Math/vec4.h"
namespace tzw {
	class vec4;
	struct Particle;
class ParticleUpdateColorModule: public ParticleEmitterModule
{
public:
	virtual ~ParticleUpdateColorModule() = default;
	ParticleUpdateColorModule(vec4 fromColor, vec4 toColor);
	virtual void process(Particle * particle, ParticleEmitter * emitter);
private:
	vec4 m_fromColor;
	vec4 m_toColor;
};

} // namespace tzw