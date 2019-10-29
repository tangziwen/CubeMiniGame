#pragma once
#include "Interface/Drawable3D.h"
#include "ParticleEmitterModule.h"
namespace tzw {

class ParticleEmitter : public Drawable3D
{
public:
	enum class State
	{
		Playing,
		Pause,
		Stop,
	};
	ParticleEmitter();
	void submitDrawCmd(RenderCommand::RenderType passType) override;
	void setUpTransFormation(TransformationInfo & info) override;
	unsigned int getTypeId() override;
	void addInitModule(ParticleEmitterModule* module);
	void addUpdateModule(ParticleEmitterModule* module);
	void logicUpdate(float dt) override;
	Mesh * m_mesh{};
	void pushCommand();
	void initMesh();
	void setIsState(State state);
private:
	std::vector<ParticleEmitterModule *> m_initModule;
	std::vector<ParticleEmitterModule *> m_updateModule;
	float m_spawnRate;
	int m_maxSpawn;
	int m_spawnAmount;
	int m_currSpawn;
	float m_t;
	State m_state;
	std::list<Particle * > particleList;
};

} // namespace tzw