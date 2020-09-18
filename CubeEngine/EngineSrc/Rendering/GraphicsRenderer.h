#pragma once
#include "../Engine/EngineDef.h"
namespace tzw
{
	class GraphicsRenderer:public Singleton<GraphicsRenderer>
	{
	public:
		GraphicsRenderer();
		void render();
	
	
	};


}