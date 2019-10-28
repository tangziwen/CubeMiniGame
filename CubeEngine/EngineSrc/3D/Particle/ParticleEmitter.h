#pragma once
#include "Interface/Drawable3D.h"
#include "ParticleEmitterModule.h"
namespace tzw {

class ParticleEmitter : public Drawable3D
{
public:
	ParticleEmitter();
	void submitDrawCmd(RenderCommand::RenderType passType) override;
	void setUpTransFormation(TransformationInfo & info) override;
	unsigned int getTypeId() override;
	void addInitModule(ParticleEmitterModule module);
	void addUpdateModule(ParticleEmitterModule module);
	void logicUpdate(float dt) override;
	Mesh * m_mesh{};
	void pushCommand();
	void initMesh();
private:
	std::vector<ParticleEmitterModule> m_initModule;
	std::vector<ParticleEmitterModule> m_updateModule;
	float m_spawnRate;
	int m_maxSpawn;
	int m_spawnAmount;
	int m_currSpawn;
	float m_t;
	std::list<Particle * > particleList;
};

} // namespace tzw