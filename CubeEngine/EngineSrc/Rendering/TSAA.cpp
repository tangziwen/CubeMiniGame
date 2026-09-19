#include "TSAA.h"

#include "BackEnd/DeviceDescriptor.h"
#include "BackEnd/DeviceMaterial.h"
#include "BackEnd/DevicePipeline.h"
#include "RenderGraph.h"
#include "Technique/MaterialPool.h"
#include "Engine/Engine.h"
#include "Base/Camera.h"
#include "BackEnd/DeviceFrameBuffer.h"
namespace tzw
{
	void TSAA::init()
	{
		if(!m_material)
		{
			m_material = new MaterialInstance();
			m_material->loadFromMaterial("TSAA");
			MaterialPool::shared()->addMaterial("TSAA", m_material);
		}
		m_attachments = {
			{ImageFormat::R16G16B16A16, false},
			{ImageFormat::D24_S8, true}
		};
        m_offset = vec2(0, 0);
		m_index = 0;
		m_targetBufferIndex = 0;
	}
    float TemporalHalton(int Index, int Base) noexcept
	{
		float Result   = 0.0f;
		float InvBase  = 1.0f / static_cast<float>(Base);
		float Fraction = InvBase;
		while (Index > 0)
		{
			Result += (Index % Base) * Fraction;
			Index /= Base;
			Fraction *= InvBase;
		}
		return Result;
	}
    void TSAA::preTick(Camera* camera)
    {
        if(!camera) return;
        m_index = (m_index + 1) % 16;
        // Restore the unjittered camera before capturing the previous-frame VP.
        camera->setOffsetPixel(0, 0);
        m_lastViewProj = camera->getViewProjectionMatrix();

        // Generate current-frame jitter in pixels and apply it for scene rendering.
        m_offset = vec2((TemporalHalton(m_index + 1, 2) - 0.5f) * m_jitterScalePixels, (TemporalHalton(m_index + 1, 3) - 0.5f) * m_jitterScalePixels);
        camera->setOffsetPixel(m_offset.x, m_offset.y);
    }
	MaterialInstance* TSAA::material() const
	{
		return m_material;
	}

	const DeviceAttachmentInfoList& TSAA::attachments() const
	{
		return m_attachments;
	}

	int TSAA::targetBufferIndex() const
	{
		return m_targetBufferIndex;
	}

	int TSAA::historyBufferIndex() const
	{
		return 1 - m_targetBufferIndex;
	}

    void TSAA::executeResolve(RenderGraphPassContext& graphContext, DeviceTexture * historyFrame, DeviceTexture * currFrame, DeviceTexture * Depth)
    {
        auto camera = graphContext.camera();
        if(!camera) return;
        // GraphicsRenderer runs TSAA after fog and before TextureToScreen. Current scene color
        // is sampled in jittered render space; reprojection uses unjittered camera space and TU_LastVP.
        // Reset before material uniforms update so TU_viewProjectInverted describes current resolve space.
        camera->setOffsetPixel(0, 0);
		auto pipeline = graphContext.pipeline();
		auto descriptor = graphContext.materialDescriptor();
		if(!pipeline || !descriptor || !historyFrame || !currFrame || !Depth)
		{
			return;
		}
        vec2 winSize = graphContext.targetFrameBuffer()->getSize();
        vec2 jitterUV = vec2(m_offset.x / winSize.x, m_offset.y / winSize.y);
        pipeline->getMat()->setVar("TU_jitterUV", jitterUV);
        pipeline->getMat()->setVar("TU_LastVP",  m_lastViewProj);
        pipeline->getMat()->setVar("TU_TSAAResolveParams", m_resolveParams);
        pipeline->getMat()->setVar("TU_TSAARejectionParams", m_rejectionParams);
        pipeline->getMat()->setVar("TU_TSAADebugMode", m_debugMode);

        graphContext.material()->updateUniform();
        descriptor->updateDescriptorByBinding(1, historyFrame);
        descriptor->updateDescriptorByBinding(2, currFrame);
        // Depth is the GBuffer depth supplied by GraphicsRenderer, not the deferred lighting/fog depth.
        descriptor->updateDescriptorByBinding(3, Depth);
        graphContext.bindSinglePipelineDescriptor();
        graphContext.drawScreenQuad();
		m_targetBufferIndex = 1 - m_targetBufferIndex;
    }
}
