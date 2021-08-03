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
    DeviceRenderStage* TSAA::draw(DeviceRenderCommand * cmd, DeviceTexture * currFrame)
    {
        std::swap(m_bufferA, m_bufferB);//swap buffer
        m_index = (m_index + 1) %(8 - 1);

        float jitterOffset = 0.8;
        //jitter the projection
        g_GetCurrScene()->defaultCamera()->setOffsetPixel((TemporalHalton(m_index + 1, 2) - 0.5f) * jitterOffset, (TemporalHalton(m_index + 1, 3) - 0.5f) * jitterOffset);

        auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
        Matrix44 proj = g_GetCurrScene()->defaultCamera()->projection();
        const float* P = proj.data();
        float R = 0.8;
        m_tsaaStage->getSinglePipeline()->getMat()->setVar("TU_RadiusInfo", vec4(R, R * R, tanf(g_GetCurrScene()->defaultCamera()->getFov() * 0.5f* 3.14 / 180.0), 0.0));
        vec4 projInfoPerspective = vec4(
            2.0f / (P[4 * 0 + 0]),                  // (x) * (R - L)/N
            2.0f / (P[4 * 1 + 1]),                  // (y) * (T - B)/N
            -(1.0f - P[4 * 2 + 0]) / P[4 * 0 + 0],  // L/N
            -(1.0f + P[4 * 2 + 1]) / P[4 * 1 + 1]  // B/N
        );
        m_tsaaStage->getSinglePipeline()->getMat()->setVar("TU_ProjInfo", projInfoPerspective);
        m_tsaaStage->prepare(cmd);
        m_tsaaStage->beginRenderPass(m_bufferA);
        m_tsaaStage->getSinglePipeline()->getMaterialDescriptorSet()->updateDescriptorByBinding(1, m_bufferB->getTextureList()[0]);
        m_tsaaStage->getSinglePipeline()->getMaterialDescriptorSet()->updateDescriptorByBinding(2, currFrame);
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