#pragma once

#include "VertexData.h"
#include <vector>
#include "Mesh.h"
#include "../Rendering/RenderBuffer.h"
#include "../Math/AABB.h"
#include "../Math/Matrix44.h"
#include "../Engine/EngineDef.h"
#include "../Math/Ray.h"
namespace tzw {
class InstancedMesh
{
public:
    InstancedMesh();
	InstancedMesh(Mesh * mesh);
	RenderBuffer *getInstanceBuf() const;
	std::vector<InstanceData> m_instanceOffset;
	void pushInstance(InstanceData instanceData);
	void pushInstances(std::vector<InstanceData> instancePos);
	void clearInstances();
	void submitInstanced(int preserveNumber = 0);
	int getInstanceSize();
	void reSubmitInstanced();
	~InstancedMesh();
	Mesh * getMesh();
	void setMesh(Mesh * mesh);
private:
	RenderBuffer* m_instanceBuf;
	Mesh * m_mesh;
};

} // namespace tzw
