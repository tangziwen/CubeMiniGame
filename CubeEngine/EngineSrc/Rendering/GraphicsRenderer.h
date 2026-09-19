#pragma once
#include "../Engine/EngineDef.h"
#include "3D/Thumbnail.h"
#include "BackEnd/DeviceRenderStage.h"
#include "RenderSettings.h"
#include "RenderGraph.h"
namespace tzw
{
	class Camera;
	class CSMShadowSystem;
	class DeviceRenderCommand;
	class MaterialInstance;
	class RenderQueue;
	class RenderPath;
	class DeviceTexture;
	class SceneView;
	class GraphicsRenderer:public Singleton<GraphicsRenderer>
	{
	public:
		GraphicsRenderer();
		void init();
		void render();
		void updateThumbNail(ThumbNail * thumb);
		std::vector<ThumbNail *> & getThumbNailList();
		RenderSettings& renderSettings();
		const RenderSettings& renderSettings() const;
		void preTick();
		SceneView* createSceneView(Camera* camera, vec2 size);
	private:
		RenderGraph m_renderGraph;
		RenderSettings m_renderSettings;
		SceneView * m_sceneView;
		std::vector<SceneView*> m_additionalSceneViews;
		CSMShadowSystem * m_csmShadowSystem;
		DeviceRenderStage * m_thumbNailRenderStage;
		RenderQueue * m_guiQueue;
		
		void initImguiStuff();
		MaterialInstance * m_imguiMat = nullptr;
		MaterialInstance* m_textureToScreenMat = nullptr;
		RenderPath * m_renderPath;
		RenderGraphResourceHandle m_imguiFont;
		std::vector<RenderGraphResourceHandle> m_screenFrameBuffers;
		std::vector<ThumbNail *> m_thumbNailList;
		RenderGraphNode buildScreenGraph(RenderGraphNode sceneOutput, int imageIndex);
		RenderGraphNode buildImguiGraph(RenderGraphResourceHandle target, RenderGraphNode previous);
		void drawPendingThumbnail(DeviceRenderCommand * cmd);
		void handleThumbNails();
		void collectUICommands();
	};


}
