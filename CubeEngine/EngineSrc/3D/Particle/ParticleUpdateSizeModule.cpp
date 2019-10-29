#include "ParticleUpdateSizeModule.h"
#include "EngineSrc/Technique/MaterialPool.h"
#include "EngineSrc/Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "Utility/math/TbaseMath.h"
#include "Particle.h"
#include "Engine/Engine.h"
#include <algorithm>

namespace tzw {
	ParticleUpdateSizeModule::ParticleUpdateSizeModule(float alphaSpeed):m_alphaSpeed(alphaSpeed)
	{
	}

	void ParticleUpdateSizeModule::process(Particle* particle)
	{
		particle->size += m_alphaSpeed * Engine::shared()->deltaTime();
		particle->size = std::min(std::max(particle->size, 0.0f), 1.0f);
	}
} // namespace tzw
