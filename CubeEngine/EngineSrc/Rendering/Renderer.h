#ifndef TZW_RENDERER_H
#define TZW_RENDERER_H

#include "RenderCommand.h"
#include <vector>
#include "RenderTarget.h"
#include "../Shader/ShaderMgr.h"
namespace tzw {
class Mesh;
class Renderer
{
public:
    Renderer();
    static Renderer * shared();
    void addRenderCommand(RenderCommand command);
    void renderAll();
    void renderAllCommon();
    void renderAllGUI();
    void renderGUI(RenderCommand &command);
    void renderCommon(RenderCommand &command);
    void clearCommands();
    void render(RenderCommand &command);
    void renderPrimitive(Mesh * mesh, ShaderProgram * program,RenderCommand::PrimitiveType primitiveType);
    bool enable3DRender() const;
    void setEnable3DRender(bool enable3DRender);

    bool enableGUIRender() const;
    void setEnableGUIRender(bool enableGUIRender);

private:
    void initQuad();
    void geometryPass();
    void LightingPass();
    void directionalLightPass();
    Mesh * m_quad;
    ShaderProgram * m_dirLightProgram;
    bool m_enable3DRender;
    bool m_enableGUIRender;
    std::vector<RenderCommand> m_GUICommandList;
    std::vector<RenderCommand> m_CommonCommand;
    static Renderer * m_instance;
    RenderTarget * m_gbuffer;
};

} // namespace tzw

#endif // TZW_RENDERER_H
