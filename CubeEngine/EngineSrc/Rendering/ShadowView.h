#pragma once

#include "Rendering/RenderView.h"

namespace tzw
{
class DeviceTexture;
class MaterialInstance;

class ShadowView : public RenderView
{
public:
	explicit ShadowView(int cascadeIndex = 0);

	void init() override;
	void collect() override;
	void draw(DeviceRenderCommand* cmd, RenderPath* renderPath) override;

	DeviceTexture* depthTexture() const;
	int cascadeIndex() const;

private:
	int m_cascadeIndex;
	DeviceRenderStage* m_shadowStage;
	MaterialInstance* m_shadowMat;
	MaterialInstance* m_shadowInstancedMat;
};
}
