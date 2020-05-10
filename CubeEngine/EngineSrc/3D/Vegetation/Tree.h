#pragma once
#include <unordered_set>

#include "../../Mesh/Mesh.h"
#include "EngineSrc/Interface/Drawable3D.h"
#include "EngineSrc/Texture/TextureMgr.h"
namespace tzw {

class TreeGroup
{
public:
  std::vector<InstanceData> m_instance;
};
class Tree : public Singleton<Tree>, public Drawable3D
{
public:
  explicit Tree();
  void addTreeGroup(TreeGroup* treeGroup);
  void clearTreeGroup();
  void finish();
  void submitDrawCmd(RenderCommand::RenderType passType) override;
  void initMesh();
  void setUpTransFormation(TransformationInfo& info) override;
  unsigned int getTypeId() override;
  Mesh* m_mesh{};
	Mesh* m_leafMesh{};
	Material * m_barkMat;
	Material * m_leafMat;
  void pushCommand();
  bool m_isFinish;

private:
  std::unordered_set<TreeGroup*> m_tree;
};

} // namespace tzw