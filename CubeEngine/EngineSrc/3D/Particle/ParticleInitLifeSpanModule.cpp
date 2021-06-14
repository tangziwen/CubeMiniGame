#include "ParticleInitLifeSpanModule.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "../../Scene/SceneMgr.h"
#include "Particle.h"
#include "Utility/math/TbaseMath.h"

namespace tzw {
	ParticleInitLifeSpanModule::ParticleInitLifeSpanModule(float lowerBound, float higherBound):m_lowerBound(lowerBound), m_higherBound(higherBound)
	{
	}

	void ParticleInitLifeSpanModule::process(Particle* particle, ParticleEmitter * emitter)
	{
		particle->m_span = TbaseMath::randRange(m_lowerBound, m_higherBound);
	}
} // namespace tzw
