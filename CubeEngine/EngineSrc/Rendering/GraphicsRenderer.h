#pragma once
#include "../Engine/EngineDef.h"
#include "3D/Thumbnail.h"
#include "vulkan/vulkan.h"
#include "BackEnd/DeviceRenderStage.h"
#include "TSAA.h"
#include "SSGI.h"
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
		void updateThumbNail(ThumbNail * thumb);
		std::vector<ThumbNail *> & getThumbNailList();
		bool m_isAAEnable = false;
		void preTick();
	private:
		TSAA m_tsaa;
		SSGI m_ssgi;
		DeviceRenderStage * m_ShadowStage[3];
		DeviceRenderStage * m_gPassStage;
		DeviceRenderStage * m_DeferredLightingStage;
		DeviceRenderStage * m_PointLightingStage;
		DeviceRenderStage * m_skyStage;
		DeviceRenderStage * m_SSRStage;
		DeviceRenderStage * m_HBAOStage;
		DeviceRenderStage * m_fogStage;
		DeviceRenderStage * m_transparentStage;
		DeviceRenderStage * m_thumbNailRenderStage;
		DeviceRenderStage * m_aaStage;
		DeviceRenderStage * m_computeTest;
		DeviceRenderStage * m_textureToScreenRenderStage[2];
		DeviceRenderStage * m_guiStage[2];
		
		DeviceBuffer *m_imguiIndex;
		DeviceBuffer *m_imguiVertex;
		DevicePipeline * m_imguiPipeline;
		void initImguiStuff();
		VkDescriptorSet m_imguiDescriptorSet;
		DeviceBufferVK * m_imguiUniformBuffer;
		Material * m_imguiMat;
		Material * m_shadowMat;
		Material * m_shadowInstancedMat;
		RenderPath * m_renderPath;
		DeviceTextureVK * m_imguiTextureFont;
		DeviceTextureVK * m_sceneCopyTex;
		std::vector<ThumbNail *> m_thumbNailList;
		void handleThumbNails();
	};


}