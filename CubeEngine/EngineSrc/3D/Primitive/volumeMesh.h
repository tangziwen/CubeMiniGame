#ifndef TZW_VOLUMEMESH_H
#define TZW_VOLUMEMESH_H
#include "../../Interface/Drawable3D.h"
#include "../../Mesh/Mesh.h"
#include <stdint.h>


namespace tzw {

class SimpleMesh : public Drawable3D
{
public:
	SimpleMesh(VertexData * vertices, uint32_t verticesSize, const uint32_t * indices, uint32_t indicesSize);
	void submitDrawCmd(RenderFlag::RenderStage stageType, RenderQueue * queues, int requirementArg) override;
	void initBuffer();
	virtual bool getIsAccpectOcTtree() const;
private:
	Mesh * m_mesh;
};
} // namespace tzw

#endif // TZW_VOLUMEMESH_H
