#include "InstancingMgr.h"

#include "Renderer.h"
#include "Scene/SceneMgr.h"

namespace tzw
{
	void InstancingMgr::prepare(RenderCommand::RenderType renderType)
	{
		for(auto & innerMap : m_map)
		{
			for(auto & t: innerMap.second)
			{
				int clearID = 0;
				switch(renderType)
				{
				case RenderCommand::RenderType::GUI: break;
				case RenderCommand::RenderType::Common:
					clearID = COMMON_PASS_INSTANCE;
					break;
				case RenderCommand::RenderType::Shadow:
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

	void InstancingMgr::pushInstanceRenderData(RenderCommand::RenderType renderType, InstanceRendereData data)
	{
		int clearID = getInstancedIndexFromRenderType(renderType);
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

	void InstancingMgr::generateDrawCall(RenderCommand::RenderType renderType)
	{
		int clearID = getInstancedIndexFromRenderType(renderType);
		for(auto & innerMap : m_map)
		{
			for(auto & t: innerMap.second)
			{
				t.second[clearID]->submitInstanced();
				RenderCommand command(t.first, innerMap.first, renderType, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
				command.setInstancedMesh(t.second[clearID]);
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
	int InstancingMgr::getInstancedIndexFromRenderType(RenderCommand::RenderType renderType)
	{
		int clearID = 0;
		switch(renderType)
		{
		case RenderCommand::RenderType::GUI: break;
		case RenderCommand::RenderType::Common:
			clearID = COMMON_PASS_INSTANCE;
			break;
		case RenderCommand::RenderType::Shadow:
			clearID = SHADOW_PASS_INSTANCE;
			break;
		default: ;
		}
		return clearID;
	}
}
