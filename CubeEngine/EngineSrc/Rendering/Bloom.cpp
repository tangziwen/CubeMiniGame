#include "Bloom.h"
#include "Technique/MaterialPool.h"
#include "EngineSrc/BackEnd/VkRenderBackEnd.h"
#include "Engine/Engine.h"
#include "EngineSrc/Scene/SceneMgr.h"
#include "BackEnd/vk/DeviceShaderCollectionVK.h"
#include "BackEnd/vk/DeviceTextureVK.h"
#include "Utility/file/Tfile.h"
#include "RenderPath.h"
namespace tzw
{
	void Bloom::init(DeviceFrameBuffer * frameBuffer)
	{

		auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());

        //bright stage
        m_brightStage = backEnd->createRenderStage_imp();
        m_brightStage->initCompute();
        m_brightStage->setName("Bright Pass");

        auto computeShader = new DeviceShaderCollectionVK();
        tzw::Data data = tzw::Tfile::shared()->getData("VulkanShaders/BrightPass.glsl",false);
        computeShader->addShader((const unsigned char *)data.getBytes(),data.getSize(),DeviceShaderType::ComputeShader,(const unsigned char *)"BrightPass.glsl");
        computeShader->finish();
        m_brightStage->createSingleComputePipeline(computeShader);

        //down Sample stage
        m_DownSampleStage = backEnd->createRenderStage_imp();
        m_DownSampleStage->initCompute();
        m_DownSampleStage->setName("DownSample Pass");

        auto downSampleShader = new DeviceShaderCollectionVK();
        tzw::Data downSampleData = tzw::Tfile::shared()->getData("VulkanShaders/DownSample.glsl",false);
        downSampleShader->addShader((const unsigned char *)downSampleData.getBytes(),downSampleData.getSize(),DeviceShaderType::ComputeShader,(const unsigned char *)"DownSample.glsl");
        downSampleShader->finish();
        m_DownSampleStage->createSingleComputePipeline(downSampleShader);
        ShadingParams * params = new ShadingParams();
        params->setVar("TU_InSize", vec2(1600, 960));
        params->setVar("TU_OutSize", vec2(1600 >> 1, 960 >> 1));
        m_DownSampleStage->getSinglePipeline()->setShadingParams(params);
        m_DownSampleStage->getSinglePipeline()->updateUniform();


        auto compositePass = backEnd->createDeviceRenderpass_imp();
        compositePass->init(1, DeviceRenderPass::OpType::LOAD_AND_STORE, ImageFormat::R16G16B16A16_SFLOAT, false);
		
        auto gBuffer = backEnd->createFrameBuffer_imp();

        m_bloomCompositeStage = backEnd->createRenderStage_imp();
        m_bloomCompositeStage->init(compositePass, frameBuffer);
        m_bloomCompositeStage->setName("Bloom Composite");
	    Material * matFog = new Material();
	    matFog->loadFromTemplate("BloomCompositePass");
	    MaterialPool::shared()->addMaterial("BloomCompositePass", matFog);
        m_bloomCompositeStage->createSinglePipeline(matFog);

        for(int i = 0; i < BLOOM_LAYERS; i ++)
        {
            auto tex = new DeviceTextureVK();
            tex->initEmpty(1600 >> i, 960 >> i, ImageFormat::R16G16B16A16,TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT);
            m_bloomTexture[i] = tex;
        }

	}


	void Bloom::draw(DeviceRenderCommand * cmd,RenderPath * path, DeviceTexture * sceneColor)
	{
        auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
        m_brightStage->prepare(cmd);
        m_brightStage->beginCompute();
        backEnd->transitionImageLayoutUseBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
        static_cast<DeviceTextureVK*>(sceneColor), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 0, 1);

        backEnd->transitionImageLayoutUseBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
        static_cast<DeviceTextureVK*>(m_bloomTexture[0]), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 0, 1);

        backEnd->transitionImageLayoutUseBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
        static_cast<DeviceTextureVK*>(m_bloomTexture[1]), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 0, 1);

        m_brightStage->getSinglePipeline()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(0, sceneColor);
        m_brightStage->getSinglePipeline()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(1, m_bloomTexture[0]);

        m_brightStage->bindSinglePipelineDescriptorCompute();
        m_brightStage->dispatch(1600/ 16, 960/ 16, 1);
        m_brightStage->endCompute();
        path->addRenderStage(m_brightStage);
        //add a barrier

        vkCmdPipelineBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
            0,0,nullptr,0,nullptr, 0, nullptr);

        m_DownSampleStage->prepare(cmd);
        m_DownSampleStage->beginCompute();
        m_DownSampleStage->getSinglePipeline()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(1, m_bloomTexture[0]);
        m_DownSampleStage->getSinglePipeline()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(2, m_bloomTexture[1]);

        m_DownSampleStage->bindSinglePipelineDescriptorCompute();
        m_DownSampleStage->dispatch((1600 >> 1)/ 16, (960 >> 1)/ 16, 1);
        m_DownSampleStage->endCompute();
        path->addRenderStage(m_DownSampleStage);
		
        backEnd->transitionImageLayoutUseBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
        static_cast<DeviceTextureVK*>(sceneColor), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0, 1);
        backEnd->transitionImageLayoutUseBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
        static_cast<DeviceTextureVK*>(m_bloomTexture[0]), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);
        backEnd->transitionImageLayoutUseBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
        static_cast<DeviceTextureVK*>(m_bloomTexture[1]), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);

        



        //composite Pass


        m_bloomCompositeStage->prepare(cmd);
        m_bloomCompositeStage->beginRenderPass();

        m_bloomCompositeStage->getSinglePipeline()->getMaterialDescriptorSet()->updateDescriptorByBinding(1, m_bloomTexture[0]);
        m_bloomCompositeStage->bindSinglePipelineDescriptor();
        m_bloomCompositeStage->drawScreenQuad();
        m_bloomCompositeStage->endRenderPass();
        m_bloomCompositeStage->finish();

        path->addRenderStage(m_bloomCompositeStage);
		return ;
	}
}