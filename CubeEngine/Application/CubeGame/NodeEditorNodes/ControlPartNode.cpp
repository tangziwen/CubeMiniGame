#include "ControlPartNode.h"
#include "CubeGame/BearPart.h"
#include "BearingPartNode.h"
#include "CubeGame/ControlPart.h"

namespace tzw
{
	ControlPartNode::ControlPartNode(ControlPart * part)
	{
		m_part = part;
		ControlPartNode::syncName();
	}

	void ControlPartNode::privateDraw()
	{
		handleNameEdit();
	}

	void ControlPartNode::onLinkOut(int startID, int endID, GameNodeEditorNode* other)
	{

	}

	void ControlPartNode::onRemoveLinkOut(int startID, int endID, GameNodeEditorNode* other)
	{

	}

	void ControlPartNode::load(rapidjson::Value& partData)
	{
		ResNode::load(partData);
	}

	void ControlPartNode::syncName()
	{
		char formatName[512];
		sprintf_s(formatName, 512, u8"×ùÎ» %s",m_part->getName().c_str());
		name = formatName;
	}

	GamePart* ControlPartNode::getProxy()
	{
		return m_part;
	}

	std::string ControlPartNode::getResType()
	{
		return "ControlPart";
	}
}
