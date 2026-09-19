#include "TSAA.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	Matrix44 TSAA::buildHistoryClipTransform(Matrix44 historyProjection, Matrix44 historyCameraTransform,
		Matrix44 projection, Matrix44 cameraTransform)
	{
		const glm::dmat4 previousCamera(glm::make_mat4(historyCameraTransform.data()));
		const glm::dmat4 currentCamera(glm::make_mat4(cameraTransform.data()));
		const glm::dmat4 previousProjection(glm::make_mat4(historyProjection.data()));
		const glm::dmat4 currentProjection(glm::make_mat4(projection.data()));
		// Cancel world translation in double precision before uploading the clip-space transform.
		const glm::dmat4 currentToPreviousView = glm::inverse(previousCamera) * currentCamera;
		const glm::dmat4 clipToHistory = previousProjection * currentToPreviousView * glm::inverse(currentProjection);
		Matrix44 result;
		for(int column = 0; column < 4; ++column)
		{
			for(int row = 0; row < 4; ++row)
			{
				result.data()[column * 4 + row] = static_cast<float>(clipToHistory[column][row]);
			}
		}
		return result;
	}

	void TSAA::init()
	{
		if(!m_material)
		{
			m_material = new MaterialInstance();
			m_material->loadFromMaterial("TSAA");
			MaterialPool::shared()->addMaterial("TSAA", m_material);
		}
		m_attachments = {
			{ImageFormat::RGBA16_UNorm, false},
			{ImageFormat::D24_UNorm_S8_UInt, true}
		};
        m_offset = vec2(0, 0);
		m_index = 0;
		m_targetBufferIndex = 0;
		resetHistory();
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
        if(!camera || m_framePending || camera != m_historyCamera)
        {
            resetHistory();
        }
        if(!camera) return;
        m_framePending = true;
        m_index = (m_index + 1) % 16;
        // Generate current-frame jitter in pixels and apply it for scene rendering.
        m_offset = vec2((TemporalHalton(m_index + 1, 2) - 0.5f) * m_jitterScalePixels, (TemporalHalton(m_index + 1, 3) - 0.5f) * m_jitterScalePixels);
        camera->setOffsetPixel(m_offset.x, m_offset.y);
    }
	void TSAA::resetHistory()
	{
		m_hasHistory = false;
		m_framePending = false;
		m_historyCamera = nullptr;
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

    void TSAA::executeResolve(RenderGraphPassContext& graphContext, DeviceTexture * historyFrame, DeviceTexture * historyDepth, DeviceTexture * currFrame, DeviceTexture * depth)
    {
        auto camera = graphContext.camera();
        if(!camera) return;
        // Resolve and history use unjittered clip space; current scene sampling removes jitter.
        camera->setOffsetPixel(0, 0);
		auto pipeline = graphContext.pipeline();
		auto descriptor = graphContext.materialDescriptor();
		if(!pipeline || !descriptor || !historyFrame || !historyDepth || !currFrame || !depth || !graphContext.targetFrameBuffer())
		{
			return;
		}
        vec2 winSize = graphContext.targetFrameBuffer()->getSize();
        Matrix44 projection = camera->projection();
        Matrix44 cameraTransform = camera->getTransform();
        bool useHistory = m_hasHistory && camera == m_historyCamera
            && winSize.x == m_historySize.x && winSize.y == m_historySize.y;
        Matrix44 clipToHistory;
        if(useHistory)
        {
            clipToHistory = buildHistoryClipTransform(m_historyProjection, m_historyCameraTransform, projection, cameraTransform);
        }
        vec2 historyClipPlanes = useHistory ? m_historyClipPlanes : vec2(camera->getNear(), camera->getFar());
        vec2 jitterUV = vec2(m_offset.x / winSize.x, m_offset.y / winSize.y);
        pipeline->getMat()->setVar("TU_jitterUV", jitterUV);
        pipeline->getMat()->setVar("TU_ClipToHistory", clipToHistory);
        pipeline->getMat()->setVar("TU_HistoryInfo", vec4(historyClipPlanes.x, historyClipPlanes.y, useHistory ? 1.0f : 0.0f, 0.0f));
        pipeline->getMat()->setVar("TU_TSAAResolveParams", m_resolveParams);
        pipeline->getMat()->setVar("TU_TSAARejectionParams", m_rejectionParams);
        pipeline->getMat()->setVar("TU_TSAADebugMode", m_debugMode);

        graphContext.material()->updateUniform();
        descriptor->updateDescriptorByBinding(1, historyFrame);
        descriptor->updateDescriptorByBinding(2, currFrame);
        // Depth is the GBuffer depth supplied by GraphicsRenderer, not the deferred lighting/fog depth.
        descriptor->updateDescriptorByBinding(3, depth);
        descriptor->updateDescriptorByBinding(4, historyDepth);
        graphContext.bindSinglePipelineDescriptor();
        graphContext.drawScreenQuad();
		m_historyProjection = projection;
		m_historyCameraTransform = cameraTransform;
		m_historyCamera = camera;
		m_historySize = winSize;
		m_historyClipPlanes = vec2(camera->getNear(), camera->getFar());
		m_hasHistory = true;
		m_framePending = false;
		m_targetBufferIndex = 1 - m_targetBufferIndex;
    }
}
