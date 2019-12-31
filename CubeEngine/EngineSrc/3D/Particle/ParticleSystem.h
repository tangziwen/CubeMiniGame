#pragma once
#include "Interface/Drawable3D.h"


//all need emitter, for further used.
#include "3D/Particle/ParticleEmitter.h"
#include "3D/Particle/ParticleInitSizeModule.h"
#include "3D/Particle/ParticleInitLifeSpanModule.h"
#include "3D/Particle/ParticleInitAlphaModule.h"
#include "3D/Particle/ParticleUpdateColorModule.h"
#include "3D/Particle/ParticleUpdateSizeModule.h"
#include "3D/Particle/ParticleSystem.h"
namespace tzw {
class ParticleEmitter;
class ParticleSystem : public Drawable3D
{
public:
	ParticleSystem();
	void logicUpdate(float dt) override;
	void addEmitter(ParticleEmitter * newEmitter);
	bool isIsRemoveAfterAllDead() const;
	void setIsRemoveAfterAllDead(const bool isRemoveAfterAllDead);
private:
	bool m_isRemoveAfterAllDead;
	std::list<ParticleEmitter * > m_emitters;
};

} // namespace tzw