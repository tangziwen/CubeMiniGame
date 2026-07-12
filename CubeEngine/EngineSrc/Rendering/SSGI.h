#pragma once

namespace tzw
{
class DeviceTexture;
class MaterialInstance;
class RenderGraphPassContext;

class SSGI
{
public:
	void init();
	void execute(RenderGraphPassContext& graphContext, DeviceTexture* currentFrame, DeviceTexture* depth,
		DeviceTexture* normal, DeviceTexture* baseColor);
	MaterialInstance* material() const;
	void preTick();

private:
	MaterialInstance* m_material = nullptr;
	int m_index = 0;
};
}
