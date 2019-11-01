#include "ParticleInitVelocityModule.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "Utility/math/TbaseMath.h"
#include "Particle.h"
#include "ParticleEmitter.h"

namespace tzw {
	ParticleInitVelocityModule::ParticleInitVelocityModule(vec3 lowerBound, vec3 higherBound):m_lowerBound(lowerBound), m_higherBound(higherBound)
	{
	}

	void ParticleInitVelocityModule::process(Particle* particle, ParticleEmitter * emitter)
	{
		float x = TbaseMath::randRange(m_lowerBound.x, m_higherBound.x);
		float y = TbaseMath::randRange(m_lowerBound.y, m_higherBound.y);
		float z = TbaseMath::randRange(m_lowerBound.z, m_higherBound.z);
		
		particle->m_velocity = emitter->getTransform().transofrmVec4(vec4(x, y, z, 0.0)).toVec3(); 
	}
} // namespace tzw
