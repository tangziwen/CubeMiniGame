#include "TSAA.h"
#include "Technique/MaterialPool.h"
#include "EngineSrc/BackEnd/VkRenderBackEnd.h"
#include "Engine/Engine.h"
#include "EngineSrc/Scene/SceneMgr.h"
namespace tzw
{
	void TSAA::init()
	{
        vec2 winSize = Engine::shared()->winSize();
        auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());

	    Material * matTSAA = new Material();
	    matTSAA->loadFromTemplate("TSAA");


	    MaterialPool::shared()->addMaterial("TSAA", matTSAA);
        auto TSAAPass = backEnd->createDeviceRenderpass_imp();
        TSAAPass->init(1, DeviceRenderPass::OpType::LOADCLEAR_AND_STORE, ImageFormat::R16G16B16A16_SFLOAT, true);

        m_tsaaStage = backEnd->createRenderStage_imp();
        //two buffer
        m_bufferA = backEnd->createFrameBuffer_imp();
        m_bufferA->init(winSize.x, winSize.y, TSAAPass);

        m_bufferB = backEnd->createFrameBuffer_imp();
        m_bufferB->init(winSize.x, winSize.y, TSAAPass);

        m_tsaaStage->init(TSAAPass, m_bufferA);
        m_tsaaStage->setName("TSAA Stage");
        m_tsaaStage->createSinglePipeline(matTSAA);
        m_offset = vec2(0, 0);
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
    void TSAA::preTick()
    {
        //last frame
        m_index = (m_index + 1) % 16;
        g_GetCurrScene()->defaultCamera()->setOffsetPixel(0, 0);
        m_lastViewProj = g_GetCurrScene()->defaultCamera()->getViewProjectionMatrix();

        //new frame
        float jitterOffset = 1.0;
        //jitter the projection
        m_offset = vec2((TemporalHalton(m_index + 1, 2) - 0.5f) * jitterOffset, (TemporalHalton(m_index + 1, 3) - 0.5f) * jitterOffset);
        g_GetCurrScene()->defaultCamera()->setOffsetPixel(m_offset.x, m_offset.y);
    }
    DeviceRenderStage* TSAA::draw(DeviceRenderCommand * cmd, DeviceTexture * currFrame, DeviceTexture * Depth)
    {
        std::swap(m_bufferA, m_bufferB);//swap buffer
        //reset jitter
        g_GetCurrScene()->defaultCamera()->setOffsetPixel(0, 0);
        auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
        m_tsaaStage->getSinglePipeline()->getMat()->setVar("TU_LastVP",  m_lastViewProj);
        
        m_tsaaStage->prepare(cmd);
        m_tsaaStage->beginRenderPass(m_bufferA);
        m_tsaaStage->getSinglePipeline()->getMaterialDescriptorSet()->updateDescriptorByBinding(1, m_bufferB->getTextureList()[0]);
        m_tsaaStage->getSinglePipeline()->getMaterialDescriptorSet()->updateDescriptorByBinding(2, currFrame);
        m_tsaaStage->getSinglePipeline()->getMaterialDescriptorSet()->updateDescriptorByBinding(3, Depth);
        m_tsaaStage->bindSinglePipelineDescriptor();
        m_tsaaStage->drawScreenQuad();
        m_tsaaStage->endRenderPass();
        m_tsaaStage->finish();
      
        return m_tsaaStage;
    }

    DeviceFrameBuffer * TSAA::getOutput()
    {
        return m_bufferA;
    }
}