#ifndef TZW_RENDERCOMMAND_H
#define TZW_RENDERCOMMAND_H

#include "../Technique/Material.h"
#include "../Mesh/Mesh.h"
#include "../Interface/DepthPolicy.h"
#include "../Math/Matrix44.h"
namespace tzw {

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
    enum class RenderType
    {
        GUI,
        Common,
		Shadow,
    };
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
    RenderCommand(Mesh * mesh,Material * material,RenderType type = RenderType::Common,PrimitiveType primitiveType = PrimitiveType::TRIANGLES, RenderBatchType batchType = RenderBatchType::Single);
    void render();

    RenderType type() const;
    void setType(const RenderType &type);
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
private:
	RenderFlag::RenderStage m_renderState;
    Mesh * m_mesh;
    Material *m_material;
    RenderType m_type;
    PrimitiveType m_primitiveType;
    unsigned int m_Zorder;
    DepthPolicy m_depthTestPolicy;
};

} // namespace tzw

#endif // TZW_RENDERCOMMAND_H
