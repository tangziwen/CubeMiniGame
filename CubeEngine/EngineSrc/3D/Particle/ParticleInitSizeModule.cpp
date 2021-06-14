#include "ParticleInitSizeModule.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "../../Scene/SceneMgr.h"
#include "Particle.h"
#include "Utility/math/TbaseMath.h"

namespace tzw {
	ParticleInitSizeModule::ParticleInitSizeModule(float lowerBound, float higherBound):m_lowerBound(lowerBound), m_higherBound(higherBound)
	{
	}

	void ParticleInitSizeModule::process(Particle* particle, ParticleEmitter * emitter)
	{
		particle->m_initSize = TbaseMath::randRange(m_lowerBound, m_higherBound);
	}
} // namespace tzw
