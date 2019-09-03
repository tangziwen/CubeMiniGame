#include "BearingPartNode.h"
#include "CubeGame/BearPart.h"

namespace tzw
{
	BearingPartNode::BearingPartNode(BearPart * bear)
	{
		m_bear = bear;
		char formatName[512];
		sprintf_s(formatName, 512, u8"轴承 %p",bear);
		name = formatName;
		addIn(u8"输入");
	}

	void BearingPartNode::privateDraw()
	{
		//control the bearing turn direction
		ImGui::Text(u8"旋转方向");
		auto click_left = false, click_right = false;
		click_left = ImGui::RadioButton(u8"左", !m_bear->m_isFlipped);ImGui::SameLine(); click_right = ImGui::RadioButton(u8"右", m_bear->m_isFlipped);
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

	void BearingPartNode::load(rapidjson::Value& partData)
	{
	}

	void BearingPartNode::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		partDocObj.AddMember("Type", std::string("Resource"), allocator);
		partDocObj.AddMember("ResType", std::string("BearPart"), allocator);
		partDocObj.AddMember("ResUID", std::string(m_bear->getGUID()), allocator);
		partDocObj.AddMember("UID", std::string(getGUID()), allocator);
	}
}
