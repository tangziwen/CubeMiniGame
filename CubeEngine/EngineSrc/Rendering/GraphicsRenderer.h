#pragma once
#include "../Engine/EngineDef.h"
#include "vulkan/vulkan.h"
namespace tzw
{
	class DevicePipelineVK;
	class DeviceBufferVK;
	class DeviceRenderStageVK;
	class Material;
	class RenderPath;
	class DeviceTextureVK;
	class GraphicsRenderer:public Singleton<GraphicsRenderer>
	{
	public:
		GraphicsRenderer();
		void init();
		void render();
	private:
		DeviceRenderStageVK * m_ShadowStage[3];
		DeviceRenderStageVK * m_gPassStage;
		DeviceRenderStageVK * m_DeferredLightingStage;
		DeviceRenderStageVK * m_skyStage;
		DeviceRenderStageVK * m_fogStage;
		DeviceRenderStageVK * m_transparentStage;
		DeviceRenderStageVK * m_thumbNailRenderStage;
		DeviceRenderStageVK * m_textureToScreenRenderStage[2];
		DeviceRenderStageVK * m_guiStage[2];
		DeviceBufferVK *m_imguiIndex;
		DeviceBufferVK *m_imguiVertex;
		DevicePipelineVK * m_imguiPipeline;
		void initImguiStuff();
		VkDescriptorSet m_imguiDescriptorSet;
		DeviceBufferVK * m_imguiUniformBuffer;
		Material * m_imguiMat;
		Material * m_shadowMat;
		Material * m_shadowInstancedMat;
		RenderPath * m_renderPath;
		DeviceTextureVK * m_imguiTextureFont;
	};


}