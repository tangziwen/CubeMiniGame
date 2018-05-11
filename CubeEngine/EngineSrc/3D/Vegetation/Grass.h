#ifndef TZW_GRASS_H
#define TZW_GRASS_H
#include "EngineSrc/Interface/Drawable3D.h"
#include "EngineSrc/Texture/TextureMgr.h"
#include "../../Mesh/Mesh.h"


namespace tzw {

class Grass : public Drawable3D
{
public:
	Grass(std::string filePath);
	void finish();
	void submitDrawCmd(RenderCommand::RenderType passType) override;
	void initMesh();
	void setUpTransFormation(TransformationInfo & info) override;
	unsigned int getTypeID() override;
	Mesh * m_mesh{};
	void pushCommand();
	bool m_isFinish;
};

} // namespace tzw

#endif // TZW_GRASS_H
