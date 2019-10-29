#pragma once
#include "ParticleEmitterModule.h"
#include "Math/vec3.h"
namespace tzw {
struct Particle;
class ParticleUpdateSizeModule: public ParticleEmitterModule
{
public:
	virtual ~ParticleUpdateSizeModule() = default;
	ParticleUpdateSizeModule(float alphaSpeed);
	virtual void process(Particle * particle);
private:
	float m_alphaSpeed;
	vec3 m_higherBound;
};

} // namespace tzw