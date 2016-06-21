#ifndef TZW_RENDERCOMMAND_H
#define TZW_RENDERCOMMAND_H

#include "../Technique/Technique.h"
#include "../Mesh/Mesh.h"
#include "../Interface/DepthPolicy.h"
namespace tzw {


class RenderCommand
{
    friend class Renderer;
public:
    ///
    /// \brief 渲染类型，不同的渲染类型在渲染时将采取不同的渲染策略
    ///
    ///
    enum class RenderType
    {
        ///GUI类型，此种类型渲染时不会测试深度也不会写入深度，其绘制顺序由Zorder决定
        GUI,
        Common
    };
    ///
    /// \brief 渲染基本几何体的类型
    ///
    enum class PrimitiveType
    {
        ///三角形
        TRIANGLES,
        ///三角链
        TRIANGLE_STRIP,
    };

    RenderCommand(Mesh * mesh,Technique * technique,RenderType type = RenderType::Common,PrimitiveType primitiveType = PrimitiveType::TRIANGLES);
    void render();

    RenderType type() const;
    void setType(const RenderType &type);

    unsigned int Zorder() const;
    void setZorder(unsigned int Zorder);

    PrimitiveType primitiveType() const;
    void setPrimitiveType(const PrimitiveType &primitiveType);

    DepthPolicy depthTestPolicy() const;
    void setDepthTestPolicy(const DepthPolicy &depthTestPolicy);

private:
    Mesh * m_mesh;
    Technique *m_technique;
    RenderType m_type;
    PrimitiveType m_primitiveType;
    unsigned int m_Zorder;
    DepthPolicy m_depthTestPolicy;
};

} // namespace tzw

#endif // TZW_RENDERCOMMAND_H
