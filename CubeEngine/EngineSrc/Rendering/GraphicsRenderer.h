#pragma once
#include "../Engine/EngineDef.h"
#include "3D/Thumbnail.h"
#include "BackEnd/DeviceRenderStage.h"
namespace tzw
{
	class CSMShadowSystem;
	class DeviceBufferVK;
	class DeviceRenderCommand;
	class MaterialInstance;
	class RenderQueue;
	class RenderPath;
	class DeviceTexture;
	class DeviceTextureVK;
	class SceneView;
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
		CSMShadowSystem * m_csmShadowSystem;
		DeviceRenderStage * m_thumbNailRenderStage;
		DeviceRenderStage * m_textureToScreenRenderStage[2];
		DeviceRenderStage * m_guiStage[2];
		RenderQueue * m_guiQueue;
		
		DeviceBuffer *m_imguiIndex;
		DeviceBuffer *m_imguiVertex;
		DevicePipeline * m_imguiPipeline;
		DeviceMaterial * m_imguiMaterial;
		void initImguiStuff();
		DeviceBufferVK * m_imguiUniformBuffer;
		MaterialInstance * m_imguiMat;
		RenderPath * m_renderPath;
		DeviceTextureVK * m_imguiTextureFont;
		std::vector<ThumbNail *> m_thumbNailList;
		void drawTextureToScreen(DeviceRenderCommand * cmd, int imageIdx, DeviceTexture * tex);
		void drawGui(DeviceRenderCommand * cmd, int imageIdx);
		void drawImgui(int imageIdx);
		void drawPendingThumbnail(DeviceRenderCommand * cmd);
		void handleThumbNails();
		void collectUICommands();
	};


}
