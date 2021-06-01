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
	enum class BillboardPolicy
	{
		ALL,
		Y_FIXED,
	};
	ParticleEmitter(int maxSpawn);
	void submitDrawCmd(RenderFlag::RenderStage requirementType, RenderQueues * queues, int requirementArg) override;
	void setUpTransFormation(TransformationInfo & info) override;
	unsigned int getTypeId() override;
	void addInitModule(ParticleEmitterModule* module);
	void addUpdateModule(ParticleEmitterModule* module);
	void logicUpdate(float dt) override;
	Mesh * m_mesh{};
	InstancedMesh * m_instancedMesh;
	void pushCommand();
	void initMesh();
	void setIsState(State state);
	void setSpawnRate(float newSpawnRate);
	float getSpawnRate() const;
	int getSpawnAmount() const;
	void setTex(std::string filePath);
	void setSpawnAmount(const int spawnAmount);
	float getDepthBias() const;
	void setBlendState(int state);
	void setDepthBias(const float depthBias);
	void setBillBoardPolicy(BillboardPolicy policy);
	State getState();
private:
	std::vector<ParticleEmitterModule *> m_initModule;
	std::vector<ParticleEmitterModule *> m_updateModule;
	float m_spawnRate;
	int m_maxSpawn;
	int m_spawnAmount;
	int m_currSpawn;
	float m_t;
	State m_state;
	bool isLocalPos;
	float m_depthBias;
	int m_historyCount;
	bool m_isInfinite;
public:
	bool isIsInfinite() const;
	void setIsInfinite(const bool isInfinite);
	bool isIsLocalPos() const;
	void setIsLocalPos(const bool isLocalPos);
private:
	std::list<Particle * > particleList;
};

} // namespace tzw