#include "SSGI.h"

#include "BackEnd/DeviceDescriptor.h"
#include "BackEnd/DeviceMaterial.h"
#include "EngineSrc/Scene/SceneMgr.h"
#include "RenderGraph.h"
#include "Technique/MaterialInstance.h"
#include "Technique/MaterialPool.h"

namespace tzw
{
void SSGI::init()
{
	if(!m_material)
	{
		m_material = new MaterialInstance();
		m_material->loadFromMaterial("SSGI");
		MaterialPool::shared()->addMaterial("SSGI", m_material);
	}
	m_index = 0;
}

void SSGI::execute(RenderGraphPassContext& graphContext, DeviceTexture* currentFrame, DeviceTexture* depth,
	DeviceTexture* normal, DeviceTexture* baseColor)
{
	auto scene = g_GetCurrScene();
	auto camera = scene ? scene->defaultCamera() : nullptr;
	auto deviceMaterial = graphContext.material();
	auto descriptor = graphContext.materialDescriptor();
	if(!m_material || !camera || !deviceMaterial || !descriptor || !currentFrame || !depth || !normal || !baseColor)
	{
		return;
	}

	m_material->setVar("TU_VP", camera->getViewProjectionMatrix());
	m_material->setVar("TU_FrameIndex", m_index);
	deviceMaterial->updateUniform();
	descriptor->updateDescriptorByBinding(1, currentFrame);
	descriptor->updateDescriptorByBinding(2, depth);
	descriptor->updateDescriptorByBinding(3, normal);
	descriptor->updateDescriptorByBinding(4, baseColor);
	graphContext.bindSinglePipelineDescriptor();
	graphContext.drawScreenQuad();
}

MaterialInstance* SSGI::material() const
{
	return m_material;
}

void SSGI::preTick()
{
	m_index = (m_index + 1) % (8 - 1);
}
}
