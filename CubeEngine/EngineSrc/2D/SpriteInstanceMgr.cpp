#include "SpriteInstanceMgr.h"
#include <algorithm>
#include "Mesh/InstancedMesh.h"
#include "Technique/MaterialPool.h"
namespace tzw {
	SpriteInstanceMgr::SpriteInstanceMgr()
	{
		setIsAccpectOcTtree(false);
		initMesh();
	}
	int SpriteInstanceMgr::addTileType(std::string filePath)
	{
		auto iter = m_tileTypeIdMap.find(filePath);
		if(iter == m_tileTypeIdMap.end())
		{
			m_totalTypes ++;//inc for type id

			auto material = new Material();
			material->loadFromTemplate("TileMap2D");
			auto texture = TextureMgr::shared()->getByPath(filePath);
			material->setTex("SpriteTexture", texture);

			

			auto instancedMesh = new InstancedMesh();
			instancedMesh->setMesh(m_mesh);

			SpriteTypeInfo * tileType = new SpriteTypeInfo();
			tileType->material = material;
			tileType->instancedMesh = instancedMesh;

			m_tileTypeIdMap[filePath] = m_totalTypes;
			m_tileTypePool[m_totalTypes] = tileType;
			return m_totalTypes;
		}
		else
		{
			return m_tileTypeIdMap[filePath];
		}
		
	}

	void SpriteInstanceMgr::initMap(int w, int h)
	{

	}

	SpriteInstanceInfo * SpriteInstanceMgr::addTile(SpriteInstanceInfo * info)
	{
		m_tilesList.emplace_back(info);
		return info;
	}

	void SpriteInstanceMgr::setOverLay(SpriteInstanceInfo *info, vec4 color)
	{
		info->overLayColor = color;
	}

	void SpriteInstanceMgr::removeTileAt(unsigned short x, unsigned short y)
	{
	}
	void SpriteInstanceMgr::removeSprite(SpriteInstanceInfo* info)
	{
		auto iter = std::find(m_tilesList.begin(), m_tilesList.end(), info);
		if(iter != m_tilesList.end())
		{
			m_tilesList.erase(iter);
		}
	}
	void SpriteInstanceMgr::initMesh()
	{
		m_mesh = new tzw::Mesh();
		m_mesh->addIndex(0);
		m_mesh->addIndex(1);
		m_mesh->addIndex(2);
		m_mesh->addIndex(0);
		m_mesh->addIndex(2);
		m_mesh->addIndex(3);

		vec2 lb(0, 0);
		vec2 rt(1,1);
		vec2 size(32.f, 32.f);
		auto width = size.x;
		auto height = size.y;
		m_mesh->clearVertices();
		auto vertex_0 = VertexData(vec3(0,0,-1),vec2(lb.x,lb.y));
		m_mesh->addVertex(vertex_0);// left bottom
		auto  vertex_1 = VertexData(vec3(width,0,-1),vec2(rt.x,lb.y));
		m_mesh->addVertex(vertex_1);// right bottom
		auto  vertex_2 = VertexData(vec3(width,height,-1),vec2(rt.x,rt.y));
		m_mesh->addVertex(vertex_2); // right top
		auto vertex_3 = VertexData(vec3(0,height,-1),vec2(lb.x,rt.y));
		m_mesh->addVertex(vertex_3); // left top
		m_mesh->finish(true);
	}

	void SpriteInstanceMgr::submitDrawCmd(RenderFlag::RenderStage renderStage, RenderQueue * queues, int requirementArg)
	{
		if(m_tilesList.empty() || !m_mesh || m_tileTypePool.empty()) return;
		for(auto& iter : m_tileTypePool)
		{
			SpriteTypeInfo * tiletype = iter.second;
			tiletype->instancedMesh->clearInstances();
		}
		for(SpriteInstanceInfo * tile : m_tilesList)
		{
			if(!tile->m_isVisible) continue;
			InstanceData data;
			data.transform.setTranslate(vec3(tile->pos.x, tile->pos.y, 0.f));
			data.extraInfo = tile->overLayColor;
			m_tileTypePool[tile->type]->instancedMesh->pushInstance(data);
		}
		auto currCam = g_GetCurrScene()->defaultGUICamera();
		Matrix44 proj = currCam->projection();
		Matrix44 view = currCam->getViewMatrix();

		Matrix44 world = getTransform();
		for(auto& iter : m_tileTypePool)
		{
			SpriteTypeInfo * tiletype = iter.second;
			//tiletype->instancedMesh->clearInstances();
			if (tiletype->instancedMesh->getInstanceSize() == 0) continue;
			tiletype->instancedMesh->submitInstanced();
			RenderCommand command(m_mesh, tiletype->material, this, RenderFlag::RenderStage::GUI, RenderCommand::PrimitiveType::TRIANGLES, RenderCommand::RenderBatchType::Instanced);
			command.setInstancedMesh(tiletype->instancedMesh);
			command.setPrimitiveType(RenderCommand::PrimitiveType::TRIANGLES);


			command.m_transInfo.m_projectMatrix = proj;
			command.m_transInfo.m_viewMatrix = view;
			command.m_transInfo.m_worldMatrix = world;

			queues->addRenderCommand(command, 0);
		}
	}

} // namespace tzw
