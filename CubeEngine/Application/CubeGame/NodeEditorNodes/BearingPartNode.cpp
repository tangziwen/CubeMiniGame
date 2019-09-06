#include "BearingPartNode.h"
#include "CubeGame/BearPart.h"

namespace tzw
{
	BearingPartNode::BearingPartNode(BearPart * bear)
	{
		m_part = bear;
		BearingPartNode::syncName();
		addIn(u8"输入");
	}

	void BearingPartNode::privateDraw()
	{
		handleNameEdit();
		//control the bearing turn direction
		ImGui::Text(u8"旋转方向");
		auto click_left = false, click_right = false;
		click_left = ImGui::RadioButton(u8"左", !m_part->m_isFlipped);ImGui::SameLine(); click_right = ImGui::RadioButton(u8"右", m_part->m_isFlipped);
		
		if(click_left) 
		{
			m_part->m_isFlipped = false;
		}
		if(click_right) 
		{
			m_part->m_isFlipped = true;
		}
		if(click_left || click_right) 
		{
			m_part->updateFlipped();        
		}
		
		auto click_steer = false, click_bear = false;
		ImGui::Text(u8"是否为舵机");
		click_steer = ImGui::RadioButton(u8"舵机", m_part->getIsSteering());ImGui::SameLine(); click_bear = ImGui::RadioButton(u8"普通轴承", !m_part->getIsSteering());
		
		if(click_steer || click_bear)
		{
			m_part->setIsSteering(click_steer);
		}
		ImGui::Text(u8"是否开启角度限制");
		bool isCurrAngleLimit;
		float angle_low;
		float angle_high;
		m_part->getAngleLimit(isCurrAngleLimit, angle_low, angle_high);
		auto click_angle_yes = false, click_angle_no = false;
		click_angle_yes = ImGui::RadioButton(u8"开启", isCurrAngleLimit);ImGui::SameLine(); click_angle_no = ImGui::RadioButton(u8"关闭", !isCurrAngleLimit);
		if(isCurrAngleLimit)
		{
			bool isInput = false;
			isInput |= ImGui::InputFloat(u8"最小角度", &angle_low);
			isInput |= ImGui::InputFloat(u8"最大角度", &angle_high);
			if(isInput)
			{
				m_part->setAngleLimit(true, angle_low, angle_high);
			}
		}
		if(click_angle_yes || click_angle_no)
		{
			m_part->setAngleLimit(click_angle_yes, angle_low, angle_high);
		}
	}

	GamePart* BearingPartNode::getProxy()
	{
		return m_part;
	}

	void BearingPartNode::load(rapidjson::Value& partData)
	{
	}

	void BearingPartNode::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		partDocObj.AddMember("Type", std::string("Resource"), allocator);
		partDocObj.AddMember("ResType", std::string("BearPart"), allocator);
		partDocObj.AddMember("ResUID", std::string(m_part->getGUID()), allocator);
		partDocObj.AddMember("UID", std::string(getGUID()), allocator);
	}

	void BearingPartNode::syncName()
	{
		char formatName[512];
		sprintf_s(formatName, 512, u8"轴承 %s",m_part->getName().c_str());
		name = formatName;
	}
}
