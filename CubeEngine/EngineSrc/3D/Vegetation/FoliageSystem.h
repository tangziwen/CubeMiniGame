#pragma once
#include <unordered_set>

#include "../../Mesh/Mesh.h"
#include "EngineSrc/Interface/Drawable3D.h"
#include "EngineSrc/Texture/TextureMgr.h"
namespace tzw {
	class Model;
	class InstancedMesh;
	class FoliageSystem;
    enum class VegetationType
    {
	    QUAD_TRI,
    	ModelType,
    	QUAD,
    };
	struct VegetationBatInfo
	{
		VegetationBatInfo();
		VegetationBatInfo(VegetationType theType, std::string filePath, vec2 size = vec2(0.8, 0.8));
		VegetationType m_type;
		std::string file_path;
		vec2 m_size;
	};
	struct VegetationBatch
	{
		VegetationBatch(const VegetationBatInfo * info);
		void insertInstanceData(InstanceData& data);
		void clear();
		void setUpTransFormation(TransformationInfo& info);
		void commitRenderCmd(RenderFlag::RenderStage stageType, RenderQueue * queues, int requirementArg);
		void commitShadowRenderCmd(RenderQueue * queues, int level);
		Model * m_model;
		Mesh * m_quadMesh;
		std::vector<InstancedMesh *> m_instancedMeshList;
		Material * m_quadMat;
		size_t m_totalCount;
		VegetationType m_type;
		VegetationBatInfo m_info;
	};
	struct VegetationInfo
	{
		VegetationInfo(std::string name);
		void init(const VegetationBatInfo * lod0, const VegetationBatInfo * lod1, const VegetationBatInfo * lod2);
		void clear();
		void commitRenderCmd(RenderFlag::RenderStage stageType, RenderQueue * queues, int requirementArg);
		void insert(InstanceData& inst);
		// VegetationType m_type;
		VegetationBatch * m_lodBatch[3];
		bool anyHas();
		void submitShadowDraw(RenderQueue * queues, int level);
		std::string m_name;
		int vegClassId = -1;
	};

	class Foliage
	{
	public:
		Foliage(int treeClass);
		std::vector<InstanceData> m_instance;
		int m_treeClass;
		void setIsVisible(bool isVisible);
		bool m_isPersistent = false;
	};

	class FoliageSystem : public Singleton<FoliageSystem>, public Drawable3D
	{
	public:
	  explicit FoliageSystem();
	  int regVegetation(VegetationInfo * vegeInfo);
	  void addTreeGroup(Foliage* treeGroup);
	  void removeFoliage(Foliage * foliage);
	  void clearTreeGroup();
	  void finish();
	  void submitDrawCmd(RenderFlag::RenderStage requirementType, RenderQueue * queues, int requirementArg) override;
	  void initMesh();
	  void setUpTransFormation(TransformationInfo& info) override;
	  void submitShadowDraw(RenderQueue * queues, int level);
	  VegetationInfo* getVegetationInfoByName(std::string name);
	  unsigned int getTypeId() override;
	  Mesh* m_mesh{};
		Mesh* m_leafMesh{};
		Material * m_barkMat;
		Material * m_leafMat;
	  void pushCommand(RenderFlag::RenderStage requirementType, RenderQueue * queues, int requirementArg);
	  bool m_isFinish;

	private:
		std::unordered_map<int, std::unordered_set<Foliage*>> m_tree;
		std::vector<VegetationInfo *> m_infoList;
	};

} // namespace tzw