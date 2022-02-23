#pragma once
#include "../Engine/EngineDef.h"
#include <deque>
#include <string>
#include "Mesh/Mesh.h"
#include "../Scene/SceneMgr.h"
#include <vector>
namespace tzw {


struct Tile2D
{
	unsigned short x = 0;
	unsigned short y = 0;
	int type = 0;
	vec4 overLayColor {1.0f, 1.0f, 1.0f, 0.0f};
};

struct TileTypeInfo
{
	Material * material = nullptr;
	InstancedMesh * instancedMesh = nullptr;
};

class TileMap2DMgr:public Drawable3D
{
public:
	TileMap2DMgr();
	int addTileType(std::string filePath);
	void initMap(int w, int h);
	Tile2D * addTile(int type, unsigned short x, unsigned short y);
	Tile2D * addTile(std::string filePath, unsigned short x, unsigned short y);
	void setOverLay(unsigned short x, unsigned short y, vec4 color);
	void removeTileAt(unsigned short x, unsigned short y);
	void initMesh();
	void submitDrawCmd(RenderFlag::RenderStage renderStage, RenderQueue * queues, int requirementArg);
	int getIndex(unsigned short x, unsigned short y);
private:
	int m_totalTypes = 0;
    Mesh * m_mesh;
	std::vector<Tile2D * > m_tilesList;
	std::unordered_map<std::string, int> m_tileTypeIdMap;
	std::unordered_map<int, TileTypeInfo *> m_tileTypePool;
	unsigned short m_width;
	unsigned short m_height;
	Tile2D * m_tileBuff;
};

} // namespace tzw

