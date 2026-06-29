#ifndef TZW_RENDERCOMMAND_H
#define TZW_RENDERCOMMAND_H
#include "../Interface/DepthPolicy.h"
#include "../Math/Matrix44.h"
#include "Mesh/InstanceData.h"
#include "Rendering/DrawPass.h"
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
	DrawPassTypeMask drawPassMask = DrawPassType::Unset;
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
    RenderCommand(Mesh * mesh,MaterialInstance * material, void * obj, DrawPassTypeMask drawPassMask, PrimitiveType primitiveType = PrimitiveType::TRIANGLES, RenderBatchType batchType = RenderBatchType::Single);
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
	DrawPassTypeMask getDrawPassMask() const;
	void setDrawPassMask(DrawPassTypeMask drawPassMask);
	void addDrawPass(DrawPassTypeMask drawPassMask);
	void removeDrawPass(DrawPassTypeMask drawPassMask);
	bool hasDrawPass(DrawPassTypeMask drawPassMask) const;
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
	DrawPassTypeMask m_drawPassMask;
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
