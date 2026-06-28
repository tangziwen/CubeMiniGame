#pragma once
#include "../Engine/EngineDef.h"
#include "3D/ShadowMap/ShadowMap.h"
#include "3D/Thumbnail.h"
#include "vulkan/vulkan.h"
#include "BackEnd/DeviceRenderStage.h"
namespace tzw
{
	class DevicePipelineVK;
	class DeviceBufferVK;
	class DeviceRenderStageVK;
	class MaterialInstance;
	class RenderQueue;
	class RenderPath;
	class DeviceTextureVK;
	class SceneView;
	class ShadowView;
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
		SceneView * m_sceneView;
		ShadowView * m_shadowViews[SHADOWMAP_CASCADE_NUM];
		DeviceRenderStage * m_thumbNailRenderStage;
		DeviceRenderStage * m_textureToScreenRenderStage[2];
		DeviceRenderStage * m_guiStage[2];
		RenderQueue * m_guiQueue;
		
		DeviceBuffer *m_imguiIndex;
		DeviceBuffer *m_imguiVertex;
		DevicePipeline * m_imguiPipeline;
		DeviceMaterial * m_imguiMaterial;
		void initImguiStuff();
		VkDescriptorSet m_imguiDescriptorSet;
		DeviceBufferVK * m_imguiUniformBuffer;
		MaterialInstance * m_imguiMat;
		RenderPath * m_renderPath;
		DeviceTextureVK * m_imguiTextureFont;
		std::vector<ThumbNail *> m_thumbNailList;
		void handleThumbNails();
		void collectUICommands();
	};


}
