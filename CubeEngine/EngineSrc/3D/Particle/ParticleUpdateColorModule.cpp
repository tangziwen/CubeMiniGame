#include "ParticleUpdateColorModule.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "Utility/math/TbaseMath.h"
#include "Particle.h"
#include "Engine/Engine.h"
#include <algorithm>

namespace tzw {
	ParticleUpdateColorModule::ParticleUpdateColorModule(vec4 fromColor, vec4 toColor):
	m_fromColor(fromColor),m_toColor(toColor)
	{
	}

	void ParticleUpdateColorModule::process(Particle* particle)
	{
		float factor = particle->m_curAge / particle->m_span;
		particle->m_color = m_fromColor * (1.0f - factor) + m_toColor * factor;
	}
} // namespace tzw
