#include "ParticleSystem.h"
#include "ParticleEmitter.h"
namespace tzw {
	ParticleSystem::ParticleSystem()
	{
		m_isRemoveAfterAllDead = false;
	}

	void ParticleSystem::logicUpdate(float dt)
	{
		if(m_isRemoveAfterAllDead && getIsValid())
		{
			bool amIDead = true;
			for (auto emitter : m_emitters)
			{
				amIDead = amIDead && (emitter->getState() == ParticleEmitter::State::Stop);
			}
			if(amIDead)
			{
				setIsValid(false);
			}
		}
	}

	void ParticleSystem::addEmitter(ParticleEmitter* newEmitter)
	{
		m_emitters.push_back(newEmitter);
		addChild(newEmitter);
	}

	bool ParticleSystem::isIsRemoveAfterAllDead() const
	{
		return m_isRemoveAfterAllDead;
	}

	void ParticleSystem::setIsRemoveAfterAllDead(const bool isRemoveAfterAllDead)
	{
		m_isRemoveAfterAllDead = isRemoveAfterAllDead;
	}
} // namespace tzw
