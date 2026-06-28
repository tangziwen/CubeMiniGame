#pragma once

#include "Bloom.h"
#include "OutlinePass.h"
#include "Rendering/RenderView.h"
#include "SSGI.h"
#include "TSAA.h"
#include <vector>

namespace tzw
{
class DeviceTexture;
class DeviceTextureVK;

class SceneView : public RenderView
{
public:
	SceneView();

	void init() override;
	void collect() override;
	void draw(DeviceRenderCommand* cmd, RenderPath* renderPath) override;
	void preTick(bool isAAEnable);
	void setAAEnabled(bool isAAEnable);
	void setShadowTextures(const std::vector<DeviceTexture*>& shadowTextures);

	DeviceTexture* outputTexture() const;
	DeviceRenderStage* gPassStage() const;
	RenderQueue* sceneQueue();

private:
	TSAA m_tsaa;
	SSGI m_ssgi;
	Bloom m_bloom;
	OutlinePass m_outlinePass;
	DeviceRenderStage * m_gPassStage;
	DeviceRenderStage * m_DeferredLightingStage;
	DeviceRenderStage * m_PointLightingStage;
	DeviceRenderStage * m_skyStage;
	DeviceRenderStage * m_debugWireframeStage;
	DeviceRenderStage * m_SSRStage;
	DeviceRenderStage * m_HBAOStage;
	DeviceRenderStage * m_fogStage;
	DeviceRenderStage * m_transparentStage;
	DeviceRenderStage * m_aaStage;
	DeviceRenderStage * m_computeTest;
	DeviceTextureVK * m_sceneCopyTex;
	DeviceTexture * m_outputTexture;
	std::vector<DeviceTexture*> m_shadowTextures;
	bool m_isAAEnable;
};
}
