#pragma once

#include "3D/ShadowMap/ShadowMap.h"
#include <vector>

namespace tzw
{
class DeviceRenderCommand;
class DeviceTexture;
class RenderPath;
class ShadowView;

class CSMShadowSystem
{
public:
	CSMShadowSystem();

	void init();
	void collect();
	void draw(DeviceRenderCommand* cmd, RenderPath* renderPath);
	const std::vector<DeviceTexture*>& depthTextures() const;

private:
	ShadowView* m_shadowViews[SHADOWMAP_CASCADE_NUM];
	std::vector<DeviceTexture*> m_depthTextures;
};
}
