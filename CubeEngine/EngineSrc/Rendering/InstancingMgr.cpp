#include "InstancingMgr.h"

#include "Scene/SceneMgr.h"
#include "Mesh/InstancedMesh.h"
#include <unordered_set>
namespace tzw
{
	void InstancingMgr::prepare(DrawPassTypeMask, int batchIdx)
	{
		bool isAllBatch = batchIdx < 0;

		for(auto innerMatMapIter : m_map)
		{
			for(auto & innerMap : innerMatMapIter.second)
			{
				for(auto & t: innerMap.second)
				{
					if(isAllBatch)
					{
						for(int i = 0; i < MAX_BATCHING_COUNT; i++)
						{
							if(t.second[i])
							{
								t.second[i]->clearInstances();
							}
						}
					}
					else
					{
						if(t.second[batchIdx])
						{
							t.second[batchIdx]->clearInstances();
						}
					}

				}
			}
		}
	}

	void InstancingMgr::pushInstanceRenderData(DrawPassTypeMask drawPassMask, InstanceRendereData data, int batchID)
	{
		uint32_t drawPassID = data.drawPassMask;
		if(drawPassID == DrawPassType::Unset)
		{
			drawPassID = static_cast<uint32_t>(data.material->getMaterial()->getDrawPassType());
		}
		if(drawPassID == DrawPassType::Unset)
		{
			drawPassID = DrawPassType::GBuffer;
		}
		auto mat_to_mesh = m_map.find(drawPassID);
		InstancedMesh * instacing = nullptr;
		if(mat_to_mesh != m_map.end())//already have this draw pass
		{
			auto mesh_to_instance = mat_to_mesh->second.find(data.material);
			
			if(mesh_to_instance != mat_to_mesh->second.end())// already have this MaterialInstance
			{
				auto instancedMesh = mesh_to_instance->second.find(data.m_mesh);
				if(instancedMesh != mesh_to_instance->second.end()) //have this material and mesh
				{
					if(instancedMesh->second[batchID])//already have this instancing
					{
						instacing = instancedMesh->second[batchID];
					}
					else//no instancing.
					{
						instacing = new InstancedMesh(data.m_mesh);
						instancedMesh->second[batchID] = instacing;
						instancedMesh->second[batchID]->pushInstance(data.data);
					}
					
				}
				else//have this material and not find this mesh
				{
					instacing = new InstancedMesh(data.m_mesh);
					for(int j = 0; j < MAX_BATCHING_COUNT; j++)
					{
						mesh_to_instance->second[data.m_mesh][j] = nullptr;
					}
					mesh_to_instance->second[data.m_mesh][batchID] = instacing;
				}
			}
			else //inner_mat_empty
			{
				innerMeshMap newMap{};
				mat_to_mesh->second.insert(std::make_pair(data.material, newMap));// = newMap;
				instacing = new InstancedMesh(data.m_mesh);

				for(int j = 0; j < MAX_BATCHING_COUNT; j++)
				{
					mat_to_mesh->second[data.material][data.m_mesh][j] = nullptr;
				}

				mat_to_mesh->second[data.material][data.m_mesh][batchID] = instacing;
			}
		}
		else //total empty
		{
			innerMatMap newMatMap{};
			m_map.insert(std::make_pair(drawPassID, newMatMap));// = newMap;
			innerMeshMap newMeshMap{};
			m_map[drawPassID].insert(std::make_pair(data.material, newMeshMap));// = newMap;
			instacing = new InstancedMesh(data.m_mesh);

			for(int j = 0; j < MAX_BATCHING_COUNT; j++)
			{
				m_map[drawPassID][data.material][data.m_mesh][j] = nullptr;
			}

			m_map[drawPassID][data.material][data.m_mesh][batchID] = instacing;
		}
		
		instacing->pushInstance(data.data);
	}

	void InstancingMgr::generateDrawCall(DrawPassTypeMask requirementDrawPassMask, RenderQueue * queues,int batchID, int requirementArg)
	{
		std::vector<RenderCommand> cmdList;
		generateDrawCall(requirementDrawPassMask, batchID, requirementArg, cmdList);

		for(auto & cmd : cmdList)
		{
			queues->addRenderCommand(cmd, requirementArg);
		}
	}

	void InstancingMgr::generateDrawCall(DrawPassTypeMask, int batchID, int requirementArg, std::vector<RenderCommand>& cmmdList)
	{
		for(auto innerMatIter : m_map)
		{
			for(auto & innerMap : innerMatIter.second)
			{
				for(auto & t: innerMap.second)
				{
					bool isNeedFullBatchCommit = batchID < 0;
					if(isNeedFullBatchCommit)
					{
						for(int i = 0; i < MAX_BATCHING_COUNT; i++)
						{
							if(t.second[i] && t.second[i]->getInstanceSize()> 0)
							{
								t.second[i]->submitInstanced();
								RenderCommand command(t.first, innerMap.first, nullptr, (DrawPassTypeMask)innerMatIter.first, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
								command.setInstancedMesh(t.second[i]);
								command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
								setUpTransFormation(command.m_transInfo);
								cmmdList.push_back(command);
							}
						}
					}
					else
					{
						if(t.second[batchID] &&t.second[batchID]->getInstanceSize()> 0)
						{
							t.second[batchID]->submitInstanced();
							RenderCommand command(t.first, innerMap.first, nullptr, (DrawPassTypeMask)innerMatIter.first, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
							command.setInstancedMesh(t.second[batchID]);
							command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
							setUpTransFormation(command.m_transInfo);
							cmmdList.push_back(command);
						}
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
	void InstancingMgr::generateSingleCommand(DrawPassTypeMask requestedDrawPassMask, std::vector<InstanceRendereData> dataList, std::vector<RenderCommand> & cmdList)
	{
		std::unordered_map<Mesh *,InstancedMesh * > tmpMeshList;
		std::unordered_map<Mesh *,MaterialInstance * > tmpMatList;
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
			RenderCommand command(instancing->getMesh(), tmpMatList[instancing->getMesh()], nullptr, requestedDrawPassMask, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
			command.setInstancedMesh(instancing);
			command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);
			setUpTransFormation(command.m_transInfo);
			cmdList.emplace_back(command);
		}
		return;
	}
	int InstancingMgr::getInstancedIndexFromDrawPass(DrawPassTypeMask drawPassMask)
	{
		int clearID = 0;
		switch(drawPassMask)
		{
		case DrawPassType::GUI: break;
		case DrawPassType::GBuffer:
			clearID = GBUFFER_PASS_INSTANCE;
			break;
		case DrawPassType::Shadow:
			clearID = SHADOW_PASS_INSTANCE;
			break;
		default: ;
		}
		return clearID;
	}
}
