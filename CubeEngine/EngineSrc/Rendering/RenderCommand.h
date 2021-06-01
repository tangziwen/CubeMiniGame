#ifndef TZW_RENDERCOMMAND_H
#define TZW_RENDERCOMMAND_H
#include "../Interface/DepthPolicy.h"
#include "../Math/Matrix44.h"
#include "Mesh/InstanceData.h"
#include <vector>

namespace tzw {
class Mesh;
class Material;
class InstancedMesh;
struct InstanceRendereData
{
	InstanceData data;
    Material * material;
    Mesh * m_mesh;
};
struct TransformationInfo{
    Matrix44 m_worldMatrix;
    Matrix44 m_viewMatrix;
    Matrix44 m_projectMatrix;
	std::vector<vec3> m_instancedOffset;
};

class RenderCommand
{
    friend class Renderer;
public:
    enum class PrimitiveType
    {
		Lines,
        TRIANGLES,
        TRIANGLE_STRIP,
        PATCHES,
    };
    enum class RenderBatchType
    {
        Single,
		Instanced,
    };
    RenderCommand(Mesh * mesh,Material * material, void * obj, RenderFlag::RenderStage renderStage, PrimitiveType primitiveType = PrimitiveType::TRIANGLES, RenderBatchType batchType = RenderBatchType::Single);
    void render();

    RenderBatchType batchType() const;
	void setBatchType(const RenderBatchType &type);
    unsigned int Zorder() const;
    void setZorder(unsigned int Zorder);

    PrimitiveType primitiveType() const;
    void setPrimitiveType(const PrimitiveType &primitiveType);

    DepthPolicy depthTestPolicy() const;
    void setDepthTestPolicy(const DepthPolicy &depthTestPolicy);
    TransformationInfo m_transInfo;
	RenderFlag::RenderStage getRenderState() const;
	void setRenderState(const RenderFlag::RenderStage renderState);
	RenderBatchType m_batchType;
	InstancedMesh* getInstancedMesh() const;
	void setInstancedMesh(InstancedMesh* const instancedMesh);
    Material * getMat();
    void setMat(Material * newMat);
    Mesh * getMesh();
    void * getDrawableObj();
private:
    void *m_obj;
	RenderFlag::RenderStage m_renderState;
    Mesh * m_mesh;
	InstancedMesh * m_instancedMesh;
    Material *m_material;
    PrimitiveType m_primitiveType;
    unsigned int m_Zorder;
    DepthPolicy m_depthTestPolicy;
};

} // namespace tzw

#endif // TZW_RENDERCOMMAND_H
