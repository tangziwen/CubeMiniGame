#pragma once
#include <unordered_set>

#include "../../Mesh/Mesh.h"
#include "EngineSrc/Interface/Drawable3D.h"
#include "EngineSrc/Texture/TextureMgr.h"
namespace tzw {
	class Model;
	class InstancedMesh;
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
		void insertInstanceData(InstanceData data);
		void clear();
		void setUpTransFormation(TransformationInfo& info);
		void commitRenderCmd();
		void commitShadowRenderCmd();
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
		VegetationInfo();
		void init(const VegetationBatInfo * lod0, const VegetationBatInfo * lod1, const VegetationBatInfo * lod2);
		void clear();
		void commitRenderCmd();
		void insert(InstanceData inst);
		// VegetationType m_type;
		VegetationBatch * m_lodBatch[3];
		bool anyHas();
		void submitShadowDraw();
	};
	class TreeGroup
	{
	public:
		TreeGroup(int treeClass);
	  std::vector<InstanceData> m_instance;
		int m_treeClass;
	};
	class Tree : public Singleton<Tree>, public Drawable3D
	{
	public:
	  explicit Tree();
		int regVegetation(const VegetationBatInfo * lod0, const VegetationBatInfo * lod1, const VegetationBatInfo * lod2);
	  void addTreeGroup(TreeGroup* treeGroup);
	  void clearTreeGroup();
	  void finish();
	  void submitDrawCmd(RenderFlag::RenderStage passType) override;
	  void initMesh();
	  void setUpTransFormation(TransformationInfo& info) override;
	  void submitShadowDraw();
	  unsigned int getTypeId() override;
	  Mesh* m_mesh{};
		Mesh* m_leafMesh{};
		Material * m_barkMat;
		Material * m_leafMat;
	  void pushCommand();
	  bool m_isFinish;

	private:
		std::unordered_map<int, std::unordered_set<TreeGroup*>> m_tree;
		std::vector<VegetationInfo *> m_infoList;
	};

} // namespace tzw