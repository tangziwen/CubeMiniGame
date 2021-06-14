#include "ParticleInitPosModule.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "../../Scene/SceneMgr.h"
#include "Particle.h"
#include "Utility/math/TbaseMath.h"
#include "ParticleEmitter.h"

namespace tzw {
	ParticleInitPosModule::ParticleInitPosModule(vec3 lowerBound, vec3 higherBound):m_lowerBound(lowerBound), m_higherBound(higherBound)
	{
	}

	void ParticleInitPosModule::process(Particle* particle, ParticleEmitter * emitter)
	{
		float x = TbaseMath::randRange(m_lowerBound.x, m_higherBound.x);
		float y = TbaseMath::randRange(m_lowerBound.y, m_higherBound.y);
		float z = TbaseMath::randRange(m_lowerBound.z, m_higherBound.z);
		particle->m_pos = emitter->getTransform().transformVec3(vec3(x, y, z));
	}
} // namespace tzw
