#include "ParticleInitAlphaModule.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "Particle.h"
#include "Utility/math/TbaseMath.h"

namespace tzw {
	ParticleInitAlphaModule::ParticleInitAlphaModule(float lowerBound, float higherBound):m_lowerBound(lowerBound), m_higherBound(higherBound)
	{
	}

	void ParticleInitAlphaModule::process(Particle* particle, ParticleEmitter * emitter)
	{
		particle->m_alpha = TbaseMath::randRange(m_lowerBound, m_higherBound);
	}
} // namespace tzw
