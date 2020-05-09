#pragma once
#include "EngineSrc/Interface/Drawable3D.h"
#include "EngineSrc/Texture/TextureMgr.h"
#include "../../Mesh/Mesh.h"


namespace tzw {

class Tree : public Drawable3D
{
public:
	explicit Tree(std::string filePath);
	void finish();
	void submitDrawCmd(RenderCommand::RenderType passType) override;
	void initMesh();
	void setUpTransFormation(TransformationInfo & info) override;
	unsigned int getTypeId() override;
	Mesh * m_mesh{};
	void pushCommand();
	bool m_isFinish;
};

} // namespace tzw