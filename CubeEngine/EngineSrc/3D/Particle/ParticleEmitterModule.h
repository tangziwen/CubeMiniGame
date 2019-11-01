#pragma once

namespace tzw {
struct Particle;
class ParticleEmitter;
class ParticleEmitterModule
{
public:
	virtual ~ParticleEmitterModule() = default;
	ParticleEmitterModule();
	virtual void process(Particle * particle, ParticleEmitter * emitter);
};

} // namespace tzw