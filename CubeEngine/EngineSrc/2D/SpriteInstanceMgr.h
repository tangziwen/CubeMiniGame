#pragma once
#include "../Engine/EngineDef.h"
#include <deque>
#include <string>
#include "Mesh/Mesh.h"
#include "../Scene/SceneMgr.h"
#include <vector>
#include <array>
namespace tzw {


constexpr int SpriteInstanceTotalLayer = 10;

struct SpriteInstanceInfo
{
	vec2 pos;
	vec2 scale = {1.f, 1.f};
	int type = 0;
	vec4 overLayColor {1.0f, 1.0f, 1.0f, 0.0f};
	bool m_isVisible = true;
	unsigned int layer = 0;
	float alpha = 1.f;
};

struct SpriteTypeInfo
{
	Material * material = nullptr;
	//InstancedMesh * instancedMesh = nullptr;
	std::array<InstancedMesh *, SpriteInstanceTotalLayer> m_instances;
};

class SpriteInstanceMgr:public Drawable3D
{
public:
	SpriteInstanceMgr();
	int addTileType(std::string filePath);
	void initMap(int w, int h);
	SpriteInstanceInfo * addTile(SpriteInstanceInfo * info);
	void setOverLay(SpriteInstanceInfo *info, vec4 color);
	void removeTileAt(unsigned short x, unsigned short y);
	void removeSprite(SpriteInstanceInfo *info);
	void initMesh();
	void submitDrawCmd(RenderFlag::RenderStage renderStage, RenderQueue * queues, int requirementArg);
	int getOrAddType(const std::string & filePath);
private:
	int m_totalTypes = 0;
    Mesh * m_mesh;
	std::vector<SpriteInstanceInfo * > m_tilesList;
	std::unordered_map<std::string, int> m_tileTypeIdMap;
	std::unordered_map<int, SpriteTypeInfo *> m_tileTypePool;
	unsigned short m_width;
	unsigned short m_height;
	SpriteInstanceInfo * m_tileBuff;
};

} // namespace tzw

