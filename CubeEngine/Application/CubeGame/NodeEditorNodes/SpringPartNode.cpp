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
		sprintf_s(formatName, 512, TRC(u8"µ¯»É %s"),m_part->getName().c_str());
		name = formatName;
	}
}
