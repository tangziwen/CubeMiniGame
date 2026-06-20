#pragma once
#include "Interface/Drawable3D.h"
#include "GameItem.h"
namespace tzw
{
class Model;
class PartSurface;
class GamePart;
class GamePartRenderNode;
struct GamePartRenderInfo
	{
		Mesh * mesh;
		MaterialInstance * material;
	};
struct GamePartModelMatList{
	std::vector<MaterialInstance *> m_matList;
};
	class GamePartRenderMgr :public Singleton<GamePartRenderMgr>
	{
	public:
		GamePartRenderMgr();
		void getRenderInfo(bool isInstancing, GamePartRenderNode * part, VisualInfo visualInfo, PartSurface * surface, std::vector<GamePartRenderInfo> &infoList);
		AABB getPartLocalAABB(VisualInfo visualInfo);
		Mesh * findOrCreateSingleMesh(VisualInfo visualInfo);
		MaterialInstance * findOrCreateSingleMaterial(bool isInsatnce, GamePartRenderNode * part, PartSurface * surface);
		MaterialInstance * findOrCreateMaterialTransparent(VisualInfo visualInfo, PartSurface * surface);
		std::string getVisualTypeStr(VisualInfo visualInfo);
		std::string getSurfaceStr(PartSurface * surface);
		Model * getModel(bool isIntance, VisualInfo visualInfo);
		GamePartModelMatList& createModelMatList(Model * model, GamePartRenderNode * part);
		std::string getStateAndRenderModeStr(GamePartRenderNode * part);
	private:
		MaterialInstance * m_previewMat;
		std::unordered_map<std::string, Mesh *> m_meshMap;
		std::unordered_map<std::string, Model *> m_modelMap;
		std::unordered_map<std::string, MaterialInstance *> m_materialMap;
		std::unordered_map<std::string, MaterialInstance *> m_materialMapSingle;
		std::unordered_map<Model *, std::unordered_map<std::string, GamePartModelMatList>> m_modelToMatList;
		void processMatList(GamePartRenderNode * part, GamePartModelMatList * matList);
	};

}
