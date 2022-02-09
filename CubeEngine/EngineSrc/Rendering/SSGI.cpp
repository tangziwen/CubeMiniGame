#include "SSGI.h"
#include "Technique/MaterialPool.h"
#include "EngineSrc/BackEnd/VkRenderBackEnd.h"
#include "Engine/Engine.h"
#include "EngineSrc/Scene/SceneMgr.h"
namespace tzw
{
	void SSGI::init()
	{
        vec2 winSize = Engine::shared()->winSize();
        auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());

	    Material * matSSGI = new Material();
	    matSSGI->loadFromTemplate("SSGI");


	    MaterialPool::shared()->addMaterial("SSGI", matSSGI);
        auto ssgiPass = backEnd->createDeviceRenderpass_imp();
        ssgiPass->init({{
            ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOAD_AND_STORE, false);

        m_stage = backEnd->createRenderStage_imp();
        //two buffer
        m_bufferA = backEnd->createFrameBuffer_imp();
        m_bufferA->init(winSize.x, winSize.y, ssgiPass);

        m_bufferB = backEnd->createFrameBuffer_imp();
        m_bufferB->init(winSize.x, winSize.y, ssgiPass);

        m_stage->init(ssgiPass, m_bufferA);
        m_stage->setName("SSGI Stage");
        m_stage->createSinglePipeline(matSSGI);
	}

    void SSGI::preTick()
    {
        m_index = (m_index + 1) %(8 - 1);
       // m_lastViewProj = g_GetCurrScene()->defaultCamera()->getViewProjectionMatrix();
        //float jitterOffset = 0.15;
        //jitter the projection
        //g_GetCurrScene()->defaultCamera()->setOffsetPixel((TemporalHalton(m_index + 1, 2) - 0.5f) * jitterOffset, (TemporalHalton(m_index + 1, 3) - 0.5f) * jitterOffset);
    }

    DeviceRenderStage* SSGI::draw(DeviceRenderCommand * cmd, DeviceTexture * currFrame, DeviceTexture * Depth, DeviceTexture * normal, DeviceTexture * baseColor, DeviceFrameBuffer * OutPutTarget)
    {
        //std::swap(m_bufferA, m_bufferB);//swap buffer
        auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
        m_stage->getSinglePipeline()->getMat()->setVar("TU_VP",  g_GetCurrScene()->defaultCamera()->getViewProjectionMatrix());
        m_stage->getSinglePipeline()->getMat()->setVar("TU_FrameIndex",  m_index);
        m_stage->prepare(cmd);
        m_stage->beginRenderPass(OutPutTarget);
        m_stage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(1, currFrame);
        m_stage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(2, Depth);
        m_stage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(3, normal);
        m_stage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(4, baseColor);
        m_stage->bindSinglePipelineDescriptor();
        m_stage->drawScreenQuad();
        m_stage->endRenderPass();
        m_stage->finish();
        return m_stage;
    }

    DeviceFrameBuffer * SSGI::getOutput()
    {
        return m_bufferA;
    }
}