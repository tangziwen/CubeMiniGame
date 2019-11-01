#include "ParticleUpdateSizeModule.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "Utility/math/TbaseMath.h"
#include "Particle.h"
#include "Engine/Engine.h"
#include <algorithm>

namespace tzw {
	ParticleUpdateSizeModule::ParticleUpdateSizeModule(float fromSize, float toSize):
	m_fromSize(fromSize), m_toSize(toSize)
	{
	}

	void ParticleUpdateSizeModule::process(Particle* particle, ParticleEmitter * emitter)
	{
		float factor = particle->m_curAge / particle->m_span;
		particle->size = m_fromSize * (1.0f - factor) + m_toSize * factor;
	}
} // namespace tzw
