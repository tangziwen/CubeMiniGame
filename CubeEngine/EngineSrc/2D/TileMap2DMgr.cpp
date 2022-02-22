#include "TileMap2DMgr.h"
#include <algorithm>
#include "Mesh/InstancedMesh.h"
#include "Technique/MaterialPool.h"
namespace tzw {
	TileMap2DMgr::TileMap2DMgr()
	{
		setIsAccpectOcTtree(false);
		initMesh();
	}
	int TileMap2DMgr::addTileType(std::string filePath)
	{
		auto iter = m_materialsPool.find(filePath);
		if(iter == m_materialsPool.end())
		{
			m_totalTypes ++;//inc for type id

			auto material = new Material();
			material->loadFromTemplate("TileMap2D");
			auto texture = TextureMgr::shared()->getByPath(filePath);
			material->setTex("SpriteTexture", texture);

			

			auto instancedMesh = new InstancedMesh();
			instancedMesh->setMesh(m_mesh);

			TileTypeInfo * tileType = new TileTypeInfo();
			tileType->material = material;
			tileType->instancedMesh = instancedMesh;

			m_materialsPool[filePath] = m_totalTypes;
			m_materialsPoolFast[m_totalTypes] = tileType;
			return m_totalTypes;
		}
		else
		{
			return m_materialsPool[filePath];
		}
		
	}

	void TileMap2DMgr::initMap(int w, int h)
	{
		m_width = w;
		m_height = h;
		size_t buffSize = sizeof(Tile2D) * w * h;
		m_tileBuff = (Tile2D * )malloc(buffSize);
		Tile2D dummy;
		for(int i = 0; i < w * h; i ++)
			memcpy(m_tileBuff + i, &dummy, sizeof(Tile2D));

	}
	Tile2D * TileMap2DMgr::addTile(int type, unsigned short x, unsigned short y)
	{
		int idx = getIndex(x, y);
		Tile2D &tile = m_tileBuff[idx];
		tile.x = x;
		tile.y = y;
		tile.type = type;
		m_tilesList.emplace_back(&tile);
		return &tile;
	}
	Tile2D * TileMap2DMgr::addTile(std::string filePath, unsigned short x, unsigned short y)
	{
		int typeID = m_materialsPool[filePath];
		return addTile(typeID, x, y);
	}

	void TileMap2DMgr::setOverLay(unsigned short x, unsigned short y, vec4 color)
	{
		int idx = getIndex(x, y);
		m_tileBuff[idx].overLayColor = color;
	}

	void TileMap2DMgr::removeTileAt(unsigned short x, unsigned short y)
	{
	}
	void TileMap2DMgr::initMesh()
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

	void TileMap2DMgr::submitDrawCmd(RenderFlag::RenderStage renderStage, RenderQueue * queues, int requirementArg)
	{
		if(m_tilesList.empty() || !m_mesh || m_materialsPoolFast.empty()) return;
		for(auto& iter : m_materialsPoolFast)
		{
			TileTypeInfo * tiletype = iter.second;
			tiletype->instancedMesh->clearInstances();
		}
		for(Tile2D * tile : m_tilesList)
		{
			InstanceData data;
			data.transform.setTranslate(vec3(tile->x * 32.f, tile->y * 32.f, 0.f));
			data.extraInfo = tile->overLayColor;
			m_materialsPoolFast[tile->type]->instancedMesh->pushInstance(data);
		}
		auto currCam = g_GetCurrScene()->defaultGUICamera();
		Matrix44 proj = currCam->projection();
		Matrix44 view = currCam->getViewMatrix();

		Matrix44 world = getTransform();
		for(auto& iter : m_materialsPoolFast)
		{
			TileTypeInfo * tiletype = iter.second;
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
	int TileMap2DMgr::getIndex(unsigned short x, unsigned short y)
	{
		int tileIdx = y * m_width + x;
		return tileIdx;
	}
} // namespace tzw
