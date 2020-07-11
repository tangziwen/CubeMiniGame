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
		void getRenderInfo(bool isInstancing, VisualInfo visualInfo, PartSurface * surface, std::vector<GamePartRenderInfo> &infoList);
		AABB getPartLocalAABB(VisualInfo visualInfo);
		Mesh * findOrCreateSingleMesh(VisualInfo visualInfo);
		Material * findOrCreateMaterial(bool isInsatnce, PartSurface * surface);
		Material * findOrCreateMaterialTransparent(VisualInfo visualInfo, PartSurface * surface);
		std::string getVisualTypeStr(VisualInfo visualInfo);
		Model * getModel(bool isIntance, VisualInfo visualInfo);
	private:
		Material * m_previewMat;
		std::unordered_map<std::string, Mesh *> m_meshMap;
		std::unordered_map<std::string, Model *> m_modelMap;
		std::unordered_map<PartSurface *, Material *> m_materialMap;
		std::unordered_map<std::string, Model *> m_modelMapSingle;
		std::unordered_map<PartSurface *, Material *> m_materialMapSingle;
	};

}
