#pragma once
#include "Base/Singleton.h"
#include <unordered_map>
#include "RenderCommand.h"

namespace tzw
{
class Mesh;
class InstancedMesh;
class Material;
#define MAX_INSTANCE_INDEX 2

#define SHADOW_PASS_INSTANCE 0
#define COMMON_PASS_INSTANCE 1
typedef std::unordered_map<Mesh *, InstancedMesh*[MAX_INSTANCE_INDEX]> innerMeshMap;
class InstancingMgr :public Singleton<InstancingMgr>
	{
	public:
		void prepare(RenderCommand::RenderType renderType);
		void pushInstanceRenderData(RenderCommand::RenderType renderType, InstanceRendereData data);
		void generateDrawCall(RenderCommand::RenderType renderType);
		void setUpTransFormation(TransformationInfo& info);
		RenderCommand generateSingleCommand(std::vector<InstanceRendereData> data);
		int getInstancedIndexFromRenderType(RenderCommand::RenderType renderType);
	private:
		std::unordered_map<Material *, innerMeshMap> m_map;
	};


}
