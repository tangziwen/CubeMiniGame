#include "CannonPartNode.h"
#include "CubeGame/CannonPart.h"

namespace tzw
{
	CannonPartNode::CannonPartNode(CannonPart * cannon)
	{
		m_part = cannon;
		CannonPartNode::syncName();
	}

	void CannonPartNode::privateDraw()
	{
		handleNameEdit();
	}

	GamePart* CannonPartNode::getProxy()
	{
		return m_part;
	}

	void CannonPartNode::load(rapidjson::Value& partData)
	{
		ResNode::load(partData);
	}

	void CannonPartNode::syncName()
	{
		char formatName[512];
		sprintf_s(formatName, 512, u8"езм╡ %s",m_part->getName().c_str());
		name = formatName;
	}

	std::string CannonPartNode::getResType()
	{
		return "BearPart";
	}
}
