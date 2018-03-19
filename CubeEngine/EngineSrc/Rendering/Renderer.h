#ifndef TZW_RENDERER_H
#define TZW_RENDERER_H

#include "RenderCommand.h"
#include <vector>
#include "RenderTarget.h"
#include "EngineSrc/3D/Effect/Effect.h"
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
	void renderAllShadow();
	void renderAllTransparent();
    void renderAllGUI();
    void renderGUI(RenderCommand &command);
    void renderCommon(RenderCommand &command);
	void renderTransparent(RenderCommand & command);
    void clearCommands();
    void render(const RenderCommand &command);
    void renderPrimitive(Mesh * mesh, Material *effect, RenderCommand::PrimitiveType primitiveType, ShaderProgram * extraProgram = nullptr);
	void renderPrimitive2(Mesh * mesh, Material *effect, RenderCommand::PrimitiveType primitiveType);
    bool enable3DRender() const;
    void setEnable3DRender(bool enable3DRender);

    bool enableGUIRender() const;
    void setEnableGUIRender(bool enableGUIRender);
	void notifySortGui();
	void renderShadow(RenderCommand &command);
private:
    void initQuad();
    void geometryPass();
    void LightingPass();
	void shadowPass();

	void prepareDeferred();

	void skyBoxPass();
	void postEffectPass();
    void directionalLightPass();
	void applyRenderSetting(Material * effect);
    void applyTransform(ShaderProgram * shader, const TransformationInfo & info);
    Mesh * m_quad;
    Material * m_dirLightProgram;
	Material * m_postEffect;
    bool m_enable3DRender;
    bool m_enableGUIRender;
	bool m_isNeedSortGUI;
    std::vector<RenderCommand> m_GUICommandList;
    std::vector<RenderCommand> m_CommonCommand;
	std::vector<RenderCommand> m_transparentCommandList;
    static Renderer * m_instance;
    RenderTarget * m_gbuffer;
	RenderTarget * m_offScreenBuffer;
};

} // namespace tzw

#endif // TZW_RENDERER_H
