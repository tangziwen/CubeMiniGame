#include "BearingPartNode.h"
#include "CubeGame/BearPart.h"

namespace tzw
{
	BearingPartNode::BearingPartNode(BearPart * bear)
	{
		m_bear = bear;
		char formatName[512];
		sprintf_s(formatName, 512, "Bearing %p",bear);
		name = formatName;
		addIn("inPut");
	}

	void BearingPartNode::privateDraw()
	{
		//control the bearing turn direction
		ImGui::Text("Rotate Direction");
		auto click_left = false, click_right = false;
		click_left = ImGui::RadioButton("left", !m_bear->m_isFlipped);ImGui::SameLine(); click_right = ImGui::RadioButton("right", m_bear->m_isFlipped);
		if(click_left) 
		{
			m_bear->m_isFlipped = false;
		}
		if(click_right) 
		{
			m_bear->m_isFlipped = true;
		}
		if(click_left || click_right) 
		{
			m_bear->updateFlipped();        
		}
	}

	BearPart* BearingPartNode::getProxy()
	{
		return m_bear;
	}
}
