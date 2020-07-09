#pragma once
#include "Interface/Drawable3D.h"
#include "GameItem.h"
namespace tzw
{
class Model;
class PartSurface;

struct GamePartRenderInfo
	{
		Mesh * mesh;
		Material * material;
	};
	class GamePartRenderMgr :public Singleton<GamePartRenderMgr>
	{
	public:
		GamePartRenderMgr();
		GamePartRenderInfo getRenderInfo(VisualInfo visualInfo, PartSurface * surface);
		AABB getPartLocalAABB(VisualInfo visualInfo);
		Mesh * findOrCreateMesh(VisualInfo visualInfo);
		Material * findOrCreateMaterial(PartSurface * surface);
		std::string getVisualTypeStr(VisualInfo visualInfo);
		Model * getModel(VisualInfo visualInfo);
	private:
		VisualInfo m_visualInfo;
		std::unordered_map<std::string, Mesh *> m_meshMap;
		std::unordered_map<std::string, Model *> m_modelMap;
		std::unordered_map<PartSurface *, Material *> m_materialMap;
	};

}
