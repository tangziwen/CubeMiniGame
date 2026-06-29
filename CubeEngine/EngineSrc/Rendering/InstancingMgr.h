#pragma once
#include "Base/Singleton.h"
#include <unordered_map>
#include "RenderCommand.h"
#include "RenderFlag.h"
#include "Rendering/RenderQueues.h"
namespace tzw
{
class Mesh;
class InstancedMesh;
class MaterialInstance;
#define MAX_INSTANCE_INDEX 2

#define SHADOW_PASS_INSTANCE 0
#define GBUFFER_PASS_INSTANCE 1
#define MAX_BATCHING_COUNT 16
typedef std::unordered_map<Mesh *, InstancedMesh*[MAX_BATCHING_COUNT]> innerMeshMap;
typedef std::unordered_map<MaterialInstance *, innerMeshMap> innerMatMap;

/*
Instancing Polyciy
for each material -> each Mesh -> each draw pass(GBuffer, shadow)-> each batch is a instancing
instancing = m_map[drawPass][material][mesh][BatchIndex],
batch index usually should be 0, but in some case such as Cascade Shadow Map, we want each
level shadow map have seperate instancings.
*/
class InstancingMgr :public Singleton<InstancingMgr>
	{
	public:
		void prepare(DrawPassTypeMask drawPassMask, int batchNumber);
		void pushInstanceRenderData(DrawPassTypeMask drawPassMask, InstanceRendereData data, int batchID);
		void generateDrawCall(DrawPassTypeMask requestedDrawPassMask, RenderQueue * queues, int batchID, int requirementArg);
		void generateDrawCall(DrawPassTypeMask requestedDrawPassMask,  int batchID, int requirementArg, std::vector<RenderCommand>& cmmdList);
		void setUpTransFormation(TransformationInfo& info);
		void generateSingleCommand(DrawPassTypeMask requestedDrawPassMask, std::vector<InstanceRendereData> data, std::vector<RenderCommand> & cmdList);
		int getInstancedIndexFromDrawPass(DrawPassTypeMask drawPassMask);
	private:
		std::unordered_map<uint32_t, innerMatMap> m_map;
	};


}
