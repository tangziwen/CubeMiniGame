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
		Material * material;
	};
struct GamePartModelMatList{
	std::vector<Material *> m_matList;
};
	class GamePartRenderMgr :public Singleton<GamePartRenderMgr>
	{
	public:
		GamePartRenderMgr();
		void getRenderInfo(bool isInstancing, GamePartRenderNode * part, VisualInfo visualInfo, PartSurface * surface, std::vector<GamePartRenderInfo> &infoList);
		AABB getPartLocalAABB(VisualInfo visualInfo);
		Mesh * findOrCreateSingleMesh(VisualInfo visualInfo);
		Material * findOrCreateMaterial(bool isInsatnce, GamePartRenderNode * part, PartSurface * surface);
		Material * findOrCreateMaterialTransparent(VisualInfo visualInfo, PartSurface * surface);
		std::string getVisualTypeStr(VisualInfo visualInfo);
		std::string getSurfaceStr(PartSurface * surface);
		Model * getModel(bool isIntance, VisualInfo visualInfo);
		GamePartModelMatList& createModelMatList(Model * model, GamePartRenderNode * part);
		std::string getStateAndRenderModeStr(GamePartRenderNode * part);
	private:
		Material * m_previewMat;
		std::unordered_map<std::string, Mesh *> m_meshMap;
		std::unordered_map<std::string, Model *> m_modelMap;
		std::unordered_map<std::string, Model *> m_modelMapSingle;
		std::unordered_map<std::string, Material *> m_materialMap;
		std::unordered_map<std::string, Material *> m_materialMapSingle;
		std::unordered_map<Model *, std::unordered_map<std::string, GamePartModelMatList>> m_modelToMatList;
		void processMatList(GamePartRenderNode * part, GamePartModelMatList * matList);
	};

}
