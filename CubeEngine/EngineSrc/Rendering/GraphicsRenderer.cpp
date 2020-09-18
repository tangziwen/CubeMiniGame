#include "GraphicsRenderer.h"
#include "Engine/Engine.h"
#include "BackEnd/RenderBackEndBase.h"
#include "Scene/SceneCuller.h"
namespace tzw
{

	GraphicsRenderer::GraphicsRenderer()
	{
	}

	void GraphicsRenderer::render()
	{
        auto screenSize = Engine::shared()->winSize();
        Engine::shared()->getRenderBackEnd()->prepareFrame();
        unsigned ImageIndex = 0;
        SceneCuller::shared()->collectPrimitives();
        RenderQueues * renderQueues = SceneCuller::shared()->getRenderQueues();

        auto & commonList = renderQueues->getCommonList();
	}

}