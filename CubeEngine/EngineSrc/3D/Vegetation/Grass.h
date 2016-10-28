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
	virtual void submitDrawCmd();
	void initMesh();
private:
	Mesh * m_mesh;
};

} // namespace tzw

#endif // TZW_GRASS_H
