#pragma once
#include "../Engine/EngineDef.h"
#include "RenderPath.h"
namespace tzw
{
	class GraphicsRenderer:public Singleton<GraphicsRenderer>
	{
	public:
		GraphicsRenderer();
		void render();
	
	
	};


}