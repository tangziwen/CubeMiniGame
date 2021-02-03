#include "InstancingMgr.h"

#include "Renderer.h"
#include "Scene/SceneMgr.h"
#include "Mesh/InstancedMesh.h"
#include <unordered_set>
namespace tzw
{
	void InstancingMgr::prepare(RenderFlag::RenderStage renderType, int batchNumber)
	{
		bool isAllBatch = batchNumber < 0;

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
				if(isAllBatch)
				{
					for(int i = 0; i < MAX_BATCHING_COUNT; i++)
					{
						if(t.second[clearID][i])
						{
							t.second[clearID][i]->clearInstances();
						}
					}
				}
				else
				{
					if(t.second[clearID][batchNumber])
					{
						t.second[clearID][batchNumber]->clearInstances();
					}
				}

			}
		}
	}

	void InstancingMgr::pushInstanceRenderData(RenderFlag::RenderStage stage, InstanceRendereData data, int batchID)
	{
		int clearID = getInstancedIndexFromRenderType(stage);
		auto mesh_to_instance = m_map.find(data.material);
		InstancedMesh * instacing = nullptr;
		if(mesh_to_instance != m_map.end())// already have this Material
		{
			auto instancedMesh = mesh_to_instance->second.find(data.m_mesh);
			if(instancedMesh != mesh_to_instance->second.end()) //have this material and mesh
			{
				if(instancedMesh->second[clearID][batchID])//already have this instancing
				{
					instacing = instancedMesh->second[clearID][batchID];
				}
				else//no instancing.
				{
					instacing = new InstancedMesh(data.m_mesh);
					instancedMesh->second[clearID][batchID] = instacing;
					instancedMesh->second[clearID][batchID]->pushInstance(data.data);
				}
				
			}
			else//have this material and not find this mesh
			{
				instacing = new InstancedMesh(data.m_mesh);
				for(int i =0 ; i< MAX_INSTANCE_INDEX; i++)
				{
					for(int j = 0; j < MAX_BATCHING_COUNT; j++)
					{
						mesh_to_instance->second[data.m_mesh][i][j] = nullptr;
					}
				}
				mesh_to_instance->second[data.m_mesh][clearID][batchID] = instacing;
			}
		}else //total empty
		{
			innerMeshMap newMap;
			m_map.insert(std::make_pair(data.material, newMap));// = newMap;
			instacing = new InstancedMesh(data.m_mesh);
			for(int i =0 ; i< MAX_INSTANCE_INDEX; i++)
			{
				for(int j = 0; j < MAX_BATCHING_COUNT; j++)
				{
					m_map[data.material][data.m_mesh][i][j] = nullptr;
				}
			}
			m_map[data.material][data.m_mesh][clearID][batchID] = instacing;
		}
		instacing->pushInstance(data.data);
		
	}

	void InstancingMgr::generateDrawCall(RenderFlag::RenderStageType requirementType, RenderQueues * queues,int batchID, int requirementArg)
	{
		RenderFlag::RenderStage renderType;
		switch(requirementType){
		case RenderFlag::RenderStageType::SHADOW:
			renderType = RenderFlag::RenderStage::SHADOW;
			break;
		case RenderFlag::RenderStageType::COMMON:
			renderType = RenderFlag::RenderStage::COMMON;
			break;
		}
		int clearID = getInstancedIndexFromRenderType(renderType);
		for(auto & innerMap : m_map)
		{
			for(auto & t: innerMap.second)
			{
				bool isNeedFullBatchCommit = batchID < 0;
				if(isNeedFullBatchCommit)
				{
					for(int i = 0; i < MAX_BATCHING_COUNT; i++)
					{
						if(t.second[clearID][i] && t.second[clearID][i]->getInstanceSize()> 0)
						{
							t.second[clearID][i]->submitInstanced();
							RenderCommand command(t.first, innerMap.first, nullptr, requirementType, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
							command.setInstancedMesh(t.second[clearID][i]);
							command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
							setUpTransFormation(command.m_transInfo);
							queues->addRenderCommand(command, requirementArg);
						}
					}
				}
				else
				{
					if(t.second[clearID][batchID] &&t.second[clearID][batchID]->getInstanceSize()> 0)
					{
						t.second[clearID][batchID]->submitInstanced();
						RenderCommand command(t.first, innerMap.first, nullptr, requirementType, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
						command.setInstancedMesh(t.second[clearID][batchID]);
						command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
						setUpTransFormation(command.m_transInfo);
						queues->addRenderCommand(command, requirementArg);
					
					}

				}

			}
		}

	}
	void InstancingMgr::generateDrawCall(RenderFlag::RenderStageType requirementType, int batchID, int requirementArg, std::vector<RenderCommand>& cmmdList)
	{
		RenderFlag::RenderStage renderType;
		switch(requirementType){
		case RenderFlag::RenderStageType::SHADOW:
			renderType = RenderFlag::RenderStage::SHADOW;
			break;
		case RenderFlag::RenderStageType::COMMON:
			renderType = RenderFlag::RenderStage::COMMON;
			break;
		}
		int clearID = getInstancedIndexFromRenderType(renderType);
		for(auto & innerMap : m_map)
		{
			for(auto & t: innerMap.second)
			{
				bool isNeedFullBatchCommit = batchID < 0;
				if(isNeedFullBatchCommit)
				{
					for(int i = 0; i < MAX_BATCHING_COUNT; i++)
					{
						if(t.second[clearID][i] &&t.second[clearID][i]->getInstanceSize()> 0)
						{
							t.second[clearID][i]->submitInstanced();
							RenderCommand command(t.first, innerMap.first, nullptr, requirementType, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
							command.setInstancedMesh(t.second[clearID][i]);
							command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
							setUpTransFormation(command.m_transInfo);
							cmmdList.emplace_back(command);
						}

					}
				}
				else
				{
					if(t.second[clearID][batchID] &&t.second[clearID][batchID]->getInstanceSize()> 0)
					{
						t.second[clearID][batchID]->submitInstanced();
						RenderCommand command(t.first, innerMap.first, nullptr, requirementType, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
						command.setInstancedMesh(t.second[clearID][batchID]);
						command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
						setUpTransFormation(command.m_transInfo);
						cmmdList.emplace_back(command);
					}

				}
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
	void InstancingMgr::generateSingleCommand(RenderFlag::RenderStageType requirementType, std::vector<InstanceRendereData> dataList, std::vector<RenderCommand> & cmdList)
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
			RenderCommand command(instancing->getMesh(), tmpMatList[instancing->getMesh()], nullptr, requirementType, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
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
