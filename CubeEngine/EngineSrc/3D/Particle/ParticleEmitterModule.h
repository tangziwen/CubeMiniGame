#pragma once

namespace tzw {
struct Particle;
class ParticleEmitterModule
{
public:
	virtual ~ParticleEmitterModule() = default;
	ParticleEmitterModule();
	virtual void process(Particle * particle);
};

} // namespace tzw