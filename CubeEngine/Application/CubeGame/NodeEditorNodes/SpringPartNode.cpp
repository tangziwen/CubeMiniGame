#include "ControlPartNode.h"
#include "CubeGame/BearPart.h"
#include "SpringPartNode.h"
#include "CubeGame/SpringPart.h"

namespace tzw
{
	SpringPartNode::SpringPartNode(SpringPart * part)
	{
		m_part = part;
		SpringPartNode::syncName();
	}

	void SpringPartNode::privateDraw()
	{
		handleNameEdit();
		float stiffness, damping;
		stiffness = m_part->getStiffness();
		damping = m_part->getDamping();
		bool isInputStiffness = false, isInputDamping = false;
		isInputStiffness = ImGui::InputFloat(u8"¾¢¶ÈÏµÊý", &stiffness);
		if(isInputStiffness)
		{
			m_part->setStiffness(stiffness);
		}
		isInputDamping = ImGui::InputFloat(u8"×èÄá", &damping);
		if(isInputDamping)
		{
			m_part->setStiffness(damping);
		}
	}

	GamePart* SpringPartNode::getProxy()
	{
		return m_part;
	}

	std::string SpringPartNode::getResType()
	{
		return "SpringPart";
	}

	void SpringPartNode::onLinkOut(int startID, int endID, GameNodeEditorNode* other)
	{
		
	}

	void SpringPartNode::load(rapidjson::Value& partData)
	{
		ResNode::load(partData);
	}

	void SpringPartNode::syncName()
	{
		char formatName[512];
		sprintf_s(formatName, 512, u8"µ¯»É %s",m_part->getName().c_str());
		name = formatName;
	}
}
