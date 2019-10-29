#include "ParticleUpdateAlphaModule.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "Utility/math/TbaseMath.h"
#include "Particle.h"
#include "Engine/Engine.h"
#include <algorithm>

namespace tzw {
	ParticleUpdateAlphaModule::ParticleUpdateAlphaModule(float alphaSpeed):m_alphaSpeed(alphaSpeed)
	{
	}

	void ParticleUpdateAlphaModule::process(Particle* particle)
	{
		particle->m_alpha += m_alphaSpeed * Engine::shared()->deltaTime();
		particle->m_alpha = std::min(std::max(particle->m_alpha, 0.0f), 1.0f);
	}
} // namespace tzw
