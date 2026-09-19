#include "Bloom.h"

#include <algorithm>

#include "BackEnd/DeviceDescriptor.h"
#include "BackEnd/DeviceMaterial.h"
#include "BackEnd/DeviceShaderCollection.h"
#include "BackEnd/VkRenderBackEnd.h"
#include "BackEnd/vk/DeviceTextureVK.h"
#include "Engine/Engine.h"
#include "RenderGraph.h"
#include "Technique/MaterialInstance.h"
#include "Technique/MaterialPool.h"
#include "Technique/ShadingParams.h"
#include "Utility/file/Tfile.h"

namespace tzw
{
namespace
{
DeviceShaderCollection* loadComputeShader(VKRenderBackEnd* backEnd, const char* path, const char* debugName)
{
	auto shader = backEnd ? backEnd->createShader_imp() : nullptr;
	if(!shader)
	{
		return nullptr;
	}
	Data data = Tfile::shared()->getData(path, false);
	shader->addShader(static_cast<const unsigned char*>(data.getBytes()), data.getSize(),
		DeviceShaderType::ComputeShader, reinterpret_cast<const unsigned char*>(debugName));
	shader->finish();
	return shader;
}

bool configureComputeMaterial(RenderGraphPassContext& graphContext, ShadingParams* params,
	DeviceTexture* input, DeviceTexture* output)
{
	auto material = graphContext.material();
	auto descriptor = graphContext.materialDescriptor();
	if(!material || !descriptor || !params || !input || !output)
	{
		return false;
	}
	material->setShadingParams(params);
	material->updateMaterialDescriptorSet();
	material->updateUniform();
	descriptor->updateDescriptorByBindingAsStorageImage(1, input);
	descriptor->updateDescriptorByBindingAsStorageImage(2, output);
	graphContext.bindSinglePipelineDescriptorCompute();
	return true;
}
}

void Bloom::init(DeviceFrameBuffer*)
{
	auto backEnd = static_cast<VKRenderBackEnd*>(Engine::shared()->getRenderBackEnd());
	if(!m_compositeMaterial)
	{
		m_brightShader = loadComputeShader(backEnd, "VulkanShaders/BrightPass.glsl", "BrightPass.glsl");
		m_downSampleShader = loadComputeShader(backEnd, "VulkanShaders/DownSample.glsl", "DownSample.glsl");
		m_blurShaders[0] = loadComputeShader(backEnd, "VulkanShaders/BlurV.glsl", "BlurV.glsl");
		m_blurShaders[1] = loadComputeShader(backEnd, "VulkanShaders/BlurH.glsl", "BlurH.glsl");

		m_brightParams = new ShadingParams();
		for(int layer = 0; layer < BLOOM_LAYERS - 1; layer++)
		{
			m_downSampleParams[layer] = new ShadingParams();
		}
		for(int layer = 0; layer < BLOOM_LAYERS; layer++)
		{
			for(int direction = 0; direction < 2; direction++)
			{
				m_blurParams[layer][direction] = new ShadingParams();
			}
		}

		m_compositeMaterial = new MaterialInstance();
		m_compositeMaterial->loadFromMaterial("BloomCompositePass");
		MaterialPool::shared()->addMaterial("BloomCompositePass", m_compositeMaterial);
	}

	auto windowSize = Engine::shared()->winSize();
	if(m_size.x == windowSize.x && m_size.y == windowSize.y && m_bloomTexture[0][0])
	{
		return;
	}
	m_size = windowSize;
	m_brightParams->setVar("TU_InSize", Engine::shared()->winSize());
	m_brightParams->setVar("TU_OutSize", layerSize(0));

	for(int layer = 0; layer < BLOOM_LAYERS - 1; layer++)
	{
		m_downSampleParams[layer]->setVar("TU_InSize", layerSize(layer));
		m_downSampleParams[layer]->setVar("TU_OutSize", layerSize(layer + 1));
	}
	for(int layer = 0; layer < BLOOM_LAYERS; layer++)
	{
		for(int direction = 0; direction < 2; direction++)
		{
			m_blurParams[layer][direction]->setVar("TU_InSize", layerSize(layer));
			m_blurParams[layer][direction]->setVar("TU_OutSize", layerSize(layer));
		}
	}

	for(int layer = 0; layer < BLOOM_LAYERS; layer++)
	{
		for(int index = 0; index < 2; index++)
		{
			delete m_bloomTexture[layer][index];
			auto texture = new DeviceTextureVK();
			auto size = layerSize(layer);
			texture->initEmpty(static_cast<size_t>(size.x), static_cast<size_t>(size.y), ImageFormat::R16G16B16A16,
				TextureRoleEnum::AS_COLOR, TextureUsageEnum::SAMPLE_AND_ATTACHMENT, 1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			m_bloomTexture[layer][index] = texture;
		}
	}
}

void Bloom::executeBrightPass(RenderGraphPassContext& graphContext, DeviceTexture* sceneColor)
{
	if(configureComputeMaterial(graphContext, m_brightParams, sceneColor, m_bloomTexture[0][0]))
	{
		auto size = layerSize(0);
		graphContext.dispatch((static_cast<uint32_t>(size.x) + 15) / 16, (static_cast<uint32_t>(size.y) + 15) / 16, 1);
	}
}

void Bloom::executeDownSamplePass(RenderGraphPassContext& graphContext, int layer)
{
	if(layer < 0 || layer >= BLOOM_LAYERS - 1)
	{
		return;
	}
	if(configureComputeMaterial(graphContext, m_downSampleParams[layer], m_bloomTexture[layer][0], m_bloomTexture[layer + 1][0]))
	{
		auto size = layerSize(layer + 1);
		graphContext.dispatch((static_cast<uint32_t>(size.x) + 15) / 16, (static_cast<uint32_t>(size.y) + 15) / 16, 1);
	}
}

void Bloom::executeBlurPass(RenderGraphPassContext& graphContext, int layer, int direction)
{
	if(layer < 0 || layer >= BLOOM_LAYERS || direction < 0 || direction >= 2)
	{
		return;
	}
	auto inputIndex = direction == 0 ? 0 : 1;
	auto outputIndex = direction == 0 ? 1 : 0;
	if(configureComputeMaterial(graphContext, m_blurParams[layer][direction],
		m_bloomTexture[layer][inputIndex], m_bloomTexture[layer][outputIndex]))
	{
		auto size = layerSize(layer);
		graphContext.dispatch((static_cast<uint32_t>(size.x) + 15) / 16, (static_cast<uint32_t>(size.y) + 15) / 16, 1);
	}
}

void Bloom::executeCompositePass(RenderGraphPassContext& graphContext)
{
	auto descriptor = graphContext.materialDescriptor();
	if(!descriptor)
	{
		return;
	}
	for(int layer = 0; layer < BLOOM_LAYERS; layer++)
	{
		descriptor->updateDescriptorByBinding(layer + 1, m_bloomTexture[layer][0]);
	}
	graphContext.bindSinglePipelineDescriptor();
	graphContext.drawScreenQuad();
}

DeviceShaderCollection* Bloom::brightShader() const
{
	return m_brightShader;
}

DeviceShaderCollection* Bloom::downSampleShader() const
{
	return m_downSampleShader;
}

DeviceShaderCollection* Bloom::blurShader(int direction) const
{
	return direction >= 0 && direction < 2 ? m_blurShaders[direction] : nullptr;
}

MaterialInstance* Bloom::compositeMaterial() const
{
	return m_compositeMaterial;
}

DeviceTexture* Bloom::bloomTexture(int layer, int index) const
{
	if(layer < 0 || layer >= BLOOM_LAYERS || index < 0 || index >= 2)
	{
		return nullptr;
	}
	return m_bloomTexture[layer][index];
}

vec2 Bloom::layerSize(int index) const
{
	auto windowSize = m_size.x > 0 && m_size.y > 0 ? m_size : Engine::shared()->winSize();
	return vec2(static_cast<float>((std::max)(1, static_cast<int>(windowSize.x) >> (index + 1))),
		static_cast<float>((std::max)(1, static_cast<int>(windowSize.y) >> (index + 1))));
}
}
