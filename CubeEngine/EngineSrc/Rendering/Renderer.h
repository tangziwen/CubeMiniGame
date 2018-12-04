#ifndef TZW_RENDERER_H
#define TZW_RENDERER_H

#include "RenderCommand.h"
#include <vector>
#include "FrameBuffer.h"
#include "EngineSrc/3D/Effect/Effect.h"
namespace tzw {
class Mesh;
class Renderer
{
public:
    Renderer();
    static Renderer * shared();
    void addRenderCommand(RenderCommand& command);
    void renderAll();
    void renderAllCommon();
	void renderAllShadow(int index);
	void renderAllTransparent();
    void renderAllGUI();
    void renderGUI(RenderCommand &command);
    void renderCommon(RenderCommand &command);
	void renderTransparent(RenderCommand & command);
    void clearCommands();
    void render(const RenderCommand &command);
    void renderPrimitive(Mesh * mesh, Material *effect, RenderCommand::PrimitiveType primitiveType, ShaderProgram * extraProgram = nullptr);
	void renderPrimitveInstanced(Mesh * mesh, Material *effect, RenderCommand::PrimitiveType primitiveType);
    bool enable3DRender() const;
    void setEnable3DRender(bool enable3DRender);

    bool enableGUIRender() const;
    void setEnableGUIRender(bool enableGUIRender);
	void notifySortGui();
	void renderShadow(RenderCommand& command, int index);
	void init();
private:
    void initQuad();
	void initMaterials();
	void initBuffer();
    void geometryPass();
    void LightingPass();
	void shadowPass();

	void skyBoxPass();
	void ssaoSetup();
	void SSAOPass();
	void SSAOBlurVPass();
	void SSAOBlurHPass();
	void SSAOBlurCompossitPass();
	void FogPass();
    void directionalLightPass();
	void applyRenderSetting(Material * effect);
    void applyTransform(ShaderProgram * shader, const TransformationInfo & info);
	void bindScreenForWriting();
    Mesh * m_quad;
    Material * m_dirLightProgram;
	Material * m_postEffect;
	Material * m_blurVEffect;
	Material * m_blurHEffect;
	Material * m_ssaoCompositeEffect;
	Material * m_fogEffect;
    bool m_enable3DRender;
    bool m_enableGUIRender;
	bool m_isNeedSortGUI;
    std::vector<RenderCommand> m_GUICommandList;
    std::vector<RenderCommand> m_CommonCommand;
	std::vector<RenderCommand> m_transparentCommandList;
	std::vector<RenderCommand> m_shadowCommandList;
    static Renderer * m_instance;
    FrameBuffer * m_gbuffer;
	FrameBuffer * m_offScreenBuffer;
	FrameBuffer * m_ssaoBuffer1;
	FrameBuffer * m_ssaoBuffer2;
};

} // namespace tzw

#endif // TZW_RENDERER_H
