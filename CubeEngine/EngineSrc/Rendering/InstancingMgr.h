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
class Material;
#define MAX_INSTANCE_INDEX 2

#define SHADOW_PASS_INSTANCE 0
#define COMMON_PASS_INSTANCE 1
#define MAX_BATCHING_COUNT 16
typedef std::unordered_map<Mesh *, InstancedMesh*[MAX_INSTANCE_INDEX][MAX_BATCHING_COUNT]> innerMeshMap;

/*
Instancing Polyciy
for each material -> each Mesh -> each stage(Common, shadow)-> each batch is a instancing
instancing = m_map[material][mesh][renderStage][BatchIndex], 
batch index usually should be 0, but in some case such as Cascade Shadow Map, we want each
level shadow map have seperate instancings.
*/
class InstancingMgr :public Singleton<InstancingMgr>
	{
	public:
		void prepare(RenderFlag::RenderStage renderType, int batchNumber);
		void pushInstanceRenderData(RenderFlag::RenderStage renderType, InstanceRendereData data, int batchID);
		void generateDrawCall(RenderFlag::RenderStageType requirementType, RenderQueues * queues, int batchID, int requirementArg);
		void generateDrawCall(RenderFlag::RenderStageType requirementType,  int batchID, int requirementArg, std::vector<RenderCommand>& cmmdList);
		void setUpTransFormation(TransformationInfo& info);
		void generateSingleCommand(RenderFlag::RenderStageType requirementType, std::vector<InstanceRendereData> data, std::vector<RenderCommand> & cmdList);
		int getInstancedIndexFromRenderType(RenderFlag::RenderStage renderType);
	private:
		std::unordered_map<Material *, innerMeshMap> m_map;
	};


}
