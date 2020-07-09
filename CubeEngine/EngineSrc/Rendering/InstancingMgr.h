#pragma once
#include "Base/Singleton.h"
#include <unordered_map>
#include "RenderCommand.h"

namespace tzw
{
class Mesh;
class InstancedMesh;
class Material;

class InstancingMgr :public Singleton<InstancingMgr>
	{
	public:
		void prepare();
		void pushInstanceRenderData(InstanceRendereData data);
		void generateDrawCall();
		void setUpTransFormation(TransformationInfo& info);
		RenderCommand generateSingleCommand(std::vector<InstanceRendereData> data);
	private:
		std::unordered_map<Material *, std::unordered_map<Mesh *, InstancedMesh*>> m_map;
	};


}
