#include "CSMShadowSystem.h"

#include "3D/ShadowMap/ShadowMap.h"
#include "ShadowView.h"

namespace tzw
{
CSMShadowSystem::CSMShadowSystem()
{
	for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
	{
		m_shadowViews[i] = nullptr;
	}
}

void CSMShadowSystem::init()
{
	for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
	{
		m_shadowViews[i] = new ShadowView(i);
		m_shadowViews[i]->init();
	}
}

void CSMShadowSystem::collect()
{
	ShadowMap::shared()->calculateProjectionMatrix();
	for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
	{
		m_shadowViews[i]->collect();
	}
}

void CSMShadowSystem::draw(DeviceRenderCommand* cmd, RenderPath* renderPath)
{
	m_depthTextures.clear();
	for(int i = 0; i < SHADOWMAP_CASCADE_NUM; i++)
	{
		m_shadowViews[i]->draw(cmd, renderPath);
		m_depthTextures.emplace_back(m_shadowViews[i]->depthTexture());
	}
}

const std::vector<DeviceTexture*>& CSMShadowSystem::depthTextures() const
{
	return m_depthTextures;
}
}
