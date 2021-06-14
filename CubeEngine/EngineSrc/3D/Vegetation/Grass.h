#ifndef TZW_GRASS_H
#define TZW_GRASS_H
#include "EngineSrc/Interface/Drawable3D.h"
#include "EngineSrc/Texture/TextureMgr.h"
#include "../../Mesh/Mesh.h"


namespace tzw {

class Grass : public Drawable3D
{
public:
	explicit Grass(std::string filePath);
	void finish();
	void submitDrawCmd(RenderFlag::RenderStage stageType, RenderQueue * queues, int requirementArg) override;
	void initMesh();
	void setUpTransFormation(TransformationInfo & info) override;
	unsigned int getTypeId() override;
	Mesh * m_mesh{};
	bool m_isFinish;
};

} // namespace tzw

#endif // TZW_GRASS_H
