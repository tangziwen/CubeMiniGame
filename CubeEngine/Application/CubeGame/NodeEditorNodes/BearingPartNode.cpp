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
		
		auto click_steer = false, click_bear = false;
		ImGui::Text(u8"是否为舵机");
		click_steer = ImGui::RadioButton(u8"舵机", m_bear->getIsSteering());ImGui::SameLine(); click_bear = ImGui::RadioButton(u8"普通轴承", !m_bear->getIsSteering());
		
		if(click_steer || click_bear)
		{
			m_bear->setIsSteering(click_steer);
		}
		ImGui::Text(u8"是否开启角度限制");
		bool isCurrAngleLimit;
		float angle_low;
		float angle_high;
		m_bear->getAngleLimit(isCurrAngleLimit, angle_low, angle_high);
		auto click_angle_yes = false, click_angle_no = false;
		click_angle_yes = ImGui::RadioButton(u8"开启", isCurrAngleLimit);ImGui::SameLine(); click_angle_no = ImGui::RadioButton(u8"关闭", !isCurrAngleLimit);
		if(isCurrAngleLimit)
		{
			bool isInput = false;
			isInput |= ImGui::InputFloat(u8"最小角度", &angle_low);
			isInput |= ImGui::InputFloat(u8"最大角度", &angle_high);
			if(isInput)
			{
				m_bear->setAngleLimit(true, angle_low, angle_high);
			}
		}
		if(click_angle_yes || click_angle_no)
		{
			m_bear->setAngleLimit(click_angle_yes, angle_low, angle_high);
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
