#ifndef TZW_RENDERER_H
#define TZW_RENDERER_H

#include "RenderCommand.h"
#include <vector>
#include "FrameBuffer.h"
#include "EngineSrc/3D/Effect/Effect.h"
namespace tzw {
class Mesh;
class ThumbNail;
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
	void renderAllClearDepthTransparent();
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
	bool isSkyEnable() const;
	void setSkyEnable(const bool skyEnable);
	bool isFogEnable() const;
	void setFogEnable(const bool fogEnable);
	bool isSsaoEnable() const;
	void setSsaoEnable(const bool ssaoEnable);
	bool isBloomEnable() const;
	void setBloomEnable(const bool bloomEnable);
	bool isHdrEnable() const;
	void setHdrEnable(const bool hdrEnable);
	bool isAaEnable() const;
	void setAaEnable(const bool aaEnable);
	void initBuffer();
	void onChangeScreenSize(int newW, int newH);
	void updateThumbNail(ThumbNail * thumb);
private:
    void initQuad();
	void initMaterials();
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
	void BloomBrightPass();
	void BloomBlurVPass();
	void BloomBlurHPass();
	void BloomCompossitPass();
    void directionalLightPass();
	void autoExposurePass();
	void AAPass();
	void applyRenderSetting(Material * effect);
    void applyTransform(ShaderProgram * shader, const TransformationInfo & info);
	void toneMappingPass();
	void bindScreenForWriting();
	void copyToFrame(FrameBuffer * bufferSrc, FrameBuffer *bufferDst, Material * mat);
	void copyToScreen(FrameBuffer * bufferSrc, Material * mat);
	void setIBL(std::string diffuseMap, std::string specularMap, bool isCubeMap);
	void handleThumbNail();
    Mesh * m_quad;
    Material * m_dirLightProgram;
	Material * m_postEffect;
	Material * m_blurVEffect;
	Material * m_blurHEffect;
	Material * m_BloomBlurHEffect;
	Material * m_BloomBlurVEffect;
	Material * m_bloomBrightPassEffect;
	Material * m_BloomCompositePassEffect;
	Material * m_autoExposurePassEffect;
	Material * m_ssaoCompositeEffect;
	Material * m_fogEffect;
	Material * m_FXAAEffect;
	Material * m_copyEffect;
	Material * m_ToneMappingPassEffect;
	
    bool m_enable3DRender;
    bool m_enableGUIRender;
	bool m_isNeedSortGUI;
    std::vector<RenderCommand> m_GUICommandList;
    std::vector<RenderCommand> m_CommonCommand;
	std::vector<RenderCommand> m_transparentCommandList;
	std::vector<RenderCommand> m_shadowCommandList;
	std::vector<RenderCommand> m_clearDepthCommandList;
    static Renderer * m_instance;
    FrameBuffer * m_gbuffer;
	FrameBuffer * m_offScreenBuffer;
	FrameBuffer * m_offScreenBuffer2;
	FrameBuffer * m_ssaoBuffer1;
	FrameBuffer * m_ssaoBuffer2;
	FrameBuffer * m_bloomBuffer1;
	FrameBuffer * m_bloomBuffer2;

	//used to help implement the switch
	FrameBuffer * tempFromBuffer;

	Texture * m_envMap;
	Texture * m_specularMap;

	bool m_skyEnable;
	bool m_fogEnable;
	bool m_ssaoEnable;
	bool m_bloomEnable;
	bool m_hdrEnable;
	bool m_aaEnable;
	bool m_shadowEnable;
	std::vector<ThumbNail *> m_thumbNailList;
	
public:
	bool isShadowEnable() const;
	void setShadowEnable(const bool shadowEnable);
};

} // namespace tzw

#endif // TZW_RENDERER_H
