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
        ShadingParams * BrightParams = new ShadingParams();
        BrightParams->setVar("TU_InSize", vec2(1600, 960));
        BrightParams->setVar("TU_OutSize", getLayerSize(0));
        m_brightStage->getSolorDeviceMaterial()->setShadingParams(BrightParams);
        m_brightStage->getSolorDeviceMaterial()->updateMaterialDescriptorSet();
        m_brightStage->getSolorDeviceMaterial()->updateUniform();

        //down Sample stage
        for(int i = 0; i < BLOOM_LAYERS - 1; i ++)
        {
            m_DownSampleStage[i] = backEnd->createRenderStage_imp();
            m_DownSampleStage[i]->initCompute();
            m_DownSampleStage[i]->setName("DownSample Pass");

            auto downSampleShader = new DeviceShaderCollectionVK();
            tzw::Data downSampleData = tzw::Tfile::shared()->getData("VulkanShaders/DownSample.glsl",false);
            downSampleShader->addShader((const unsigned char *)downSampleData.getBytes(),downSampleData.getSize(),DeviceShaderType::ComputeShader,(const unsigned char *)"DownSample.glsl");
            downSampleShader->finish();
            m_DownSampleStage[i]->createSingleComputePipeline(downSampleShader);
            ShadingParams * params = new ShadingParams();
            params->setVar("TU_InSize", getLayerSize(i));
            params->setVar("TU_OutSize", getLayerSize(i + 1));
            m_DownSampleStage[i]->getSolorDeviceMaterial()->setShadingParams(params);
            m_DownSampleStage[i]->getSolorDeviceMaterial()->updateMaterialDescriptorSet();
            m_DownSampleStage[i]->getSolorDeviceMaterial()->updateUniform();
        }

        //Gaussain Blur
        for(int i = 0; i < BLOOM_LAYERS; i ++)
        {
            m_blurStage[i][0] = backEnd->createRenderStage_imp();
            m_blurStage[i][0]->initCompute();
            m_blurStage[i][0]->setName("Blur Vertical Pass");

            auto BlurVShader = new DeviceShaderCollectionVK();
            tzw::Data blurSampleData = tzw::Tfile::shared()->getData("VulkanShaders/BlurV.glsl",false);
            BlurVShader->addShader((const unsigned char *)blurSampleData.getBytes(),blurSampleData.getSize(),DeviceShaderType::ComputeShader,(const unsigned char *)"BlurV.glsl");
            BlurVShader->finish();
            m_blurStage[i][0]->createSingleComputePipeline(BlurVShader);
            ShadingParams * params = new ShadingParams();
            params->setVar("TU_InSize", getLayerSize(i));
            params->setVar("TU_OutSize", getLayerSize(i));
            m_blurStage[i][0]->getSolorDeviceMaterial()->setShadingParams(params);
            m_blurStage[i][0]->getSolorDeviceMaterial()->updateMaterialDescriptorSet();
            m_blurStage[i][0]->getSolorDeviceMaterial()->updateUniform();



            m_blurStage[i][1] = backEnd->createRenderStage_imp();
            m_blurStage[i][1]->initCompute();
            m_blurStage[i][1]->setName("Blur Horizonal Pass");

            auto BlurHShader = new DeviceShaderCollectionVK();
            tzw::Data blurHSampleData = tzw::Tfile::shared()->getData("VulkanShaders/BlurH.glsl",false);
            BlurHShader->addShader((const unsigned char *)blurHSampleData.getBytes(),blurHSampleData.getSize(),DeviceShaderType::ComputeShader,(const unsigned char *)"BlurH.glsl");
            BlurHShader->finish();
            m_blurStage[i][1]->createSingleComputePipeline(BlurHShader);
            ShadingParams * Hparams = new ShadingParams();
            Hparams->setVar("TU_InSize", getLayerSize(i));
            Hparams->setVar("TU_OutSize", getLayerSize(i));
            m_blurStage[i][1]->getSolorDeviceMaterial()->setShadingParams(Hparams);
            m_blurStage[i][1]->getSolorDeviceMaterial()->updateMaterialDescriptorSet();
            m_blurStage[i][1]->getSolorDeviceMaterial()->updateUniform();
        }

        auto compositePass = backEnd->createDeviceRenderpass_imp();
        compositePass->init({{
            ImageFormat::R16G16B16A16, false}, {ImageFormat::D24_S8, true}}, DeviceRenderPass::OpType::LOAD_AND_STORE, true);
		
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
            for(int j = 0; j < 2; j++)
            {
                auto tex = new DeviceTextureVK();
                vec2 size = getLayerSize(i);
                tex->initEmpty(size.x, size.y, ImageFormat::R16G16B16A16,TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, 1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                m_bloomTexture[i][j] = tex;
            }
        }
	}


	void Bloom::draw(DeviceRenderCommand * cmd,RenderPath * path, DeviceTexture * sceneColor)
	{

        auto backEnd = static_cast<VKRenderBackEnd *>(Engine::shared()->getRenderBackEnd());
        m_brightStage->prepare(cmd);
        m_brightStage->beginCompute();
        backEnd->transitionImageLayoutUseBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
        static_cast<DeviceTextureVK*>(sceneColor), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 0, 1);

        for(int i = 0; i < BLOOM_LAYERS; i++)
        {
            for(int j = 0; j < 2; j++)
            {
                backEnd->transitionImageLayoutUseBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
                static_cast<DeviceTextureVK*>(m_bloomTexture[i][j]), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 0, 1);
            }
        }

        m_brightStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(1, sceneColor);
        m_brightStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(2, m_bloomTexture[0][0]);

        m_brightStage->bindSinglePipelineDescriptorCompute();
        vec2 disptachSize = getLayerSize(0);
        m_brightStage->dispatch(disptachSize.x/ 16, disptachSize.y/ 16, 1);
        m_brightStage->endCompute();
        path->addRenderStage(m_brightStage);
        //add a barrier

        vkCmdPipelineBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
            0,0,nullptr,0,nullptr, 0, nullptr);

        for(int i = 0; i < BLOOM_LAYERS - 1; i ++)
        {

            m_DownSampleStage[i]->prepare(cmd);
            m_DownSampleStage[i]->beginCompute();
            m_DownSampleStage[i]->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(1, m_bloomTexture[i][0]);
            m_DownSampleStage[i]->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(2, m_bloomTexture[i + 1][0]);

            m_DownSampleStage[i]->bindSinglePipelineDescriptorCompute();
            vec2 disptachSize = getLayerSize(i + 1);
            m_DownSampleStage[i]->dispatch(disptachSize.x/ 16, disptachSize.y/ 16, 1);
            m_DownSampleStage[i]->endCompute();
            path->addRenderStage(m_DownSampleStage[i]);
            vkCmdPipelineBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                0,0,nullptr,0,nullptr, 0, nullptr);
        }

        //ping pong blur
        for(int i = 0; i < BLOOM_LAYERS; i++)
        {
            vec2 disptachSize = getLayerSize(i);

            //Blur V
            m_blurStage[i][0]->prepare(cmd);
            m_blurStage[i][0]->beginCompute();
            m_blurStage[i][0]->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(1, m_bloomTexture[i][0]);
            m_blurStage[i][0]->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(2, m_bloomTexture[i][1]);

            m_blurStage[i][0]->bindSinglePipelineDescriptorCompute();
            
            m_blurStage[i][0]->dispatch(disptachSize.x/ 16, disptachSize.y/ 16, 1);
            m_blurStage[i][0]->endCompute();
            path->addRenderStage(m_blurStage[i][0]);
            vkCmdPipelineBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                0,0,nullptr,0,nullptr, 0, nullptr);

            //Blur H
            m_blurStage[i][1]->prepare(cmd);
            m_blurStage[i][1]->beginCompute();
            m_blurStage[i][1]->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(1, m_bloomTexture[i][1]);
            m_blurStage[i][1]->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBindingAsStorageImage(2, m_bloomTexture[i][0]);

            m_blurStage[i][1]->bindSinglePipelineDescriptorCompute();
            m_blurStage[i][1]->dispatch(disptachSize.x/ 16, disptachSize.y/ 16, 1);
            m_blurStage[i][1]->endCompute();
            path->addRenderStage(m_blurStage[i][1]);
            vkCmdPipelineBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                0,0,nullptr,0,nullptr, 0, nullptr);
        }

		
        backEnd->transitionImageLayoutUseBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
        static_cast<DeviceTextureVK*>(sceneColor), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0, 1);

        for(int i = 0; i < BLOOM_LAYERS; i++)
        {
            for(int j = 0; j < 2; j++)
            {
                backEnd->transitionImageLayoutUseBarrier(static_cast<DeviceRenderCommandVK *>(cmd)->getVK(), 
                static_cast<DeviceTextureVK*>(m_bloomTexture[i][j]), VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);
            }
        }

        



        //composite Pass


        m_bloomCompositeStage->prepare(cmd);
        m_bloomCompositeStage->beginRenderPass();

        m_bloomCompositeStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(1, m_bloomTexture[0][0]);
        m_bloomCompositeStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(2, m_bloomTexture[1][0]);
        m_bloomCompositeStage->getSolorDeviceMaterial()->getMaterialDescriptorSet()->updateDescriptorByBinding(3, m_bloomTexture[2][0]);
        m_bloomCompositeStage->bindSinglePipelineDescriptor();
        m_bloomCompositeStage->drawScreenQuad();
        m_bloomCompositeStage->endRenderPass();
        m_bloomCompositeStage->finish();

        path->addRenderStage(m_bloomCompositeStage);
		return ;
	}
    vec2 Bloom::getLayerSize(int index)
    {
        return vec2(1600 >> (index + 1), 960 >> (index + 1));
    }
}