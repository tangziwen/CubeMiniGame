#ifndef TZW_RENDERCOMMAND_H
#define TZW_RENDERCOMMAND_H
#include "../Interface/DepthPolicy.h"
#include "../Math/Matrix44.h"
#include "Mesh/InstanceData.h"
#include "Rendering/RenderFlag.h"
#include <vector>

namespace tzw {
class Mesh;
class MaterialInstance;
class InstancedMesh;
struct InstanceRendereData
{
	InstanceData data;
    MaterialInstance * material;
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
    RenderCommand(Mesh * mesh,MaterialInstance * material, void * obj, RenderFlag::RenderStage renderStage, PrimitiveType primitiveType = PrimitiveType::TRIANGLES, RenderBatchType batchType = RenderBatchType::Single);
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
	uint32_t getRenderStageMask() const;
	void setRenderStageMask(uint32_t renderStageMask);
	void addRenderStage(RenderFlag::RenderStage renderStage);
	void removeRenderStage(RenderFlag::RenderStage renderStage);
	bool hasRenderStage(RenderFlag::RenderStage renderStage) const;
	RenderBatchType m_batchType;
	InstancedMesh* getInstancedMesh() const;
	void setInstancedMesh(InstancedMesh* const instancedMesh);
    MaterialInstance * getMat();
    void setMat(MaterialInstance * newMat);
    Mesh * getMesh();
    void * getDrawableObj();
	void setOutlineColor(const vec4& outlineColor);
	const vec4& outlineColor() const;
private:
    void *m_obj;
	uint32_t m_renderStageMask;
    Mesh * m_mesh;
	InstancedMesh * m_instancedMesh;
    MaterialInstance *m_material;
    PrimitiveType m_primitiveType;
    unsigned int m_Zorder;
    DepthPolicy m_depthTestPolicy;
	vec4 m_outlineColor;
};

} // namespace tzw

#endif // TZW_RENDERCOMMAND_H
