#include "InstancingMgr.h"

#include "Renderer.h"
#include "Scene/SceneMgr.h"
#include "Mesh/InstancedMesh.h"
#include <unordered_set>
namespace tzw
{
	void InstancingMgr::prepare(RenderFlag::RenderStage renderType)
	{
		for(auto & innerMap : m_map)
		{
			for(auto & t: innerMap.second)
			{
				int clearID = 0;
				switch(renderType)
				{
				case RenderFlag::RenderStage::GUI: break;
				case RenderFlag::RenderStage::COMMON:
					clearID = COMMON_PASS_INSTANCE;
					break;
				case RenderFlag::RenderStage::SHADOW:
					clearID = SHADOW_PASS_INSTANCE;
					break;
				default: ;
				}
				if(t.second[clearID])
				{
					t.second[clearID]->clearInstances();
				}
			}
		}
	}

	void InstancingMgr::pushInstanceRenderData(RenderFlag::RenderStage stage, InstanceRendereData data)
	{
		int clearID = getInstancedIndexFromRenderType(stage);
		auto mesh_to_instance = m_map.find(data.material);
		InstancedMesh * instacing = nullptr;
		if(mesh_to_instance != m_map.end())
		{
			auto instancedMesh = mesh_to_instance->second.find(data.m_mesh);
			if(instancedMesh != mesh_to_instance->second.end())
			{
				if(instancedMesh->second[clearID])
				{
					instacing = instancedMesh->second[clearID];
				}
				else
				{
					instacing = new InstancedMesh(data.m_mesh);
					instancedMesh->second[clearID] = instacing;
					instancedMesh->second[clearID]->pushInstance(data.data);
				}
				
			}else
			{
				instacing = new InstancedMesh(data.m_mesh);
				for(int i =0 ; i< MAX_INSTANCE_INDEX; i++)
				{
					mesh_to_instance->second[data.m_mesh][i] = nullptr;
				}
				mesh_to_instance->second[data.m_mesh][clearID] = instacing;
			}
		}else
		{
			innerMeshMap newMap;
			m_map.insert(std::make_pair(data.material, newMap));// = newMap;
			instacing = new InstancedMesh(data.m_mesh);
			for(int i =0 ; i< MAX_INSTANCE_INDEX; i++)
			{
				m_map[data.material][data.m_mesh][i] = nullptr;
			}
			m_map[data.material][data.m_mesh][clearID] = instacing;
		}
		instacing->pushInstance(data.data);
		
	}

	void InstancingMgr::generateDrawCall(RenderFlag::RenderStage renderType)
	{
		int clearID = getInstancedIndexFromRenderType(renderType);
		for(auto & innerMap : m_map)
		{
			for(auto & t: innerMap.second)
			{
				t.second[clearID]->submitInstanced();
				RenderCommand command(t.first, innerMap.first, nullptr, innerMap.first->getRenderStage(), RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
				command.setInstancedMesh(t.second[clearID]);
				command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
				setUpTransFormation(command.m_transInfo);
				Renderer::shared()->addRenderCommand(command);
			}
		}

	}
	void InstancingMgr::generateDrawCall(RenderFlag::RenderStage renderType, std::vector<RenderCommand>& cmmdList)
	{
		int clearID = getInstancedIndexFromRenderType(renderType);
		for(auto & innerMap : m_map)
		{
			for(auto & t: innerMap.second)
			{
				t.second[clearID]->submitInstanced();
				RenderCommand command(t.first, innerMap.first, nullptr, innerMap.first->getRenderStage(), RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
				command.setInstancedMesh(t.second[clearID]);
				command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
				setUpTransFormation(command.m_transInfo);
				cmmdList.emplace_back(command);
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
	void InstancingMgr::generateSingleCommand(std::vector<InstanceRendereData> dataList, std::vector<RenderCommand> & cmdList)
	{
		std::unordered_map<Mesh *,InstancedMesh * > tmpMeshList;
		std::unordered_map<Mesh *,Material * > tmpMatList;
		for(auto data : dataList)
		{
			if(tmpMeshList.find(data.m_mesh) == tmpMeshList.end())
			{
				auto instancing = new InstancedMesh();
				instancing->setMesh(data.m_mesh);
				tmpMeshList[data.m_mesh] = instancing;
				tmpMatList[data.m_mesh] = data.material;
			}
			tmpMeshList[data.m_mesh]->pushInstance(data.data);
		}
		
		for(auto iter : tmpMeshList)
		{
			auto instancing = iter.second;
			instancing->submitInstanced();
			RenderCommand command(instancing->getMesh(), tmpMatList[instancing->getMesh()], nullptr, RenderFlag::RenderStage::COMMON, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
			command.setInstancedMesh(instancing);
			command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
			setUpTransFormation(command.m_transInfo);
			cmdList.emplace_back(command);
		}
		return;
	}
	int InstancingMgr::getInstancedIndexFromRenderType(RenderFlag::RenderStage renderType)
	{
		int clearID = 0;
		switch(renderType)
		{
		case RenderFlag::RenderStage::GUI: break;
		case RenderFlag::RenderStage::COMMON:
			clearID = COMMON_PASS_INSTANCE;
			break;
		case RenderFlag::RenderStage::SHADOW:
			clearID = SHADOW_PASS_INSTANCE;
			break;
		default: ;
		}
		return clearID;
	}
}
