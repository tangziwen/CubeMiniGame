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
		Instanced,
		Shadow,
    };
    enum class PrimitiveType
    {
		Lines,
        TRIANGLES,
        TRIANGLE_STRIP,
        PATCHES,
    };

    RenderCommand(Mesh * mesh,Material * material,RenderType type = RenderType::Common,PrimitiveType primitiveType = PrimitiveType::TRIANGLES);
    void render();

    RenderType type() const;
    void setType(const RenderType &type);

    unsigned int Zorder() const;
    void setZorder(unsigned int Zorder);

    PrimitiveType primitiveType() const;
    void setPrimitiveType(const PrimitiveType &primitiveType);

    DepthPolicy depthTestPolicy() const;
    void setDepthTestPolicy(const DepthPolicy &depthTestPolicy);
    TransformationInfo m_transInfo;
	bool getIsNeedTransparent() const;
	void setIsNeedTransparent(bool val);
private:
    Mesh * m_mesh;
    Material *m_material;
    RenderType m_type;
    PrimitiveType m_primitiveType;
    unsigned int m_Zorder;
    DepthPolicy m_depthTestPolicy;
	bool m_isNeedTransparent;
};

} // namespace tzw

#endif // TZW_RENDERCOMMAND_H
