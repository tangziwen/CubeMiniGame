#include "InstancingMgr.h"

#include "Renderer.h"
#include "Scene/SceneMgr.h"

namespace tzw
{
	void InstancingMgr::prepare()
	{
		for(auto & innerMap : m_map)
		{
			for(auto & t: innerMap.second)
			{
				t.second->clearInstances();
			}
		}
	}

	void InstancingMgr::pushInstanceRenderData(InstanceRendereData data)
	{
		auto mesh_to_instance = m_map.find(data.material);
		if(mesh_to_instance != m_map.end())
		{
			auto instancedMesh = mesh_to_instance->second.find(data.m_mesh);
			if(instancedMesh != mesh_to_instance->second.end())
			{
				instancedMesh->second->pushInstance(data.data);
			}else
			{
				auto instacing = new InstancedMesh();
				instacing->pushInstance(data.data);
				instacing->setMesh(data.m_mesh);
				mesh_to_instance->second[data.m_mesh] = instacing;
			}
		}else
		{
			std::unordered_map<Mesh *, InstancedMesh *> newMap;
			m_map[data.material] = newMap;
			auto instacing = new InstancedMesh();
			instacing->pushInstance(data.data);
			instacing->setMesh(data.m_mesh);
			newMap[data.m_mesh] = instacing;
		}
	}

	void InstancingMgr::generateDrawCall()
	{
		for(auto & innerMap : m_map)
		{
			for(auto & t: innerMap.second)
			{
				t.second->submitInstanced();
				RenderCommand command(t.first, innerMap.first, RenderCommand::RenderType::Common, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
				command.setInstancedMesh(t.second);
				command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
				setUpTransFormation(command.m_transInfo);
				Renderer::shared()->addRenderCommand(command);
			}
		}

	}
	void InstancingMgr::setUpTransFormation(TransformationInfo& info)
	{
		auto currCam = g_GetCurrScene()->defaultCamera();
		info.m_projectMatrix = currCam->projection();
		info.m_viewMatrix = currCam->getViewMatrix();
		Matrix44 mat;
		mat.setToIdentity();
		info.m_worldMatrix = mat;
	}
	RenderCommand InstancingMgr::generateSingleCommand(std::vector<InstanceRendereData> dataList)
	{
		auto instacing = new InstancedMesh();
		instacing->setMesh(dataList[0].m_mesh);
		for(auto data: dataList)
		{
			instacing->pushInstance(data.data);
		}
		instacing->submitInstanced();
		RenderCommand command(instacing->getMesh(), dataList[0].material, RenderCommand::RenderType::Common, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
		command.setInstancedMesh(instacing);
		command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
		setUpTransFormation(command.m_transInfo);
		return command;
	}
}
