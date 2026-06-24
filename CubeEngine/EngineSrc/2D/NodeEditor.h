#pragma once
#include "IMGUISystem.h"
namespace tzw
{
	
	class NodeEditor : public IMGUIObject
	{
	public:
		// 通过 IMGUIObject 继承
		void drawIMGUI() override;

	};

}