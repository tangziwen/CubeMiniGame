#include "ThrusterPartNode.h"
#include "CubeGame/ThrusterPart.h"

namespace tzw
{
	ThrusterPartNode::ThrusterPartNode(ThrusterPart * cannon)
	{
		m_part = cannon;
		ThrusterPartNode::syncName();
	}

	void ThrusterPartNode::privateDraw()
	{
		handleNameEdit();
	}

	GamePart* ThrusterPartNode::getProxy()
	{
		return m_part;
	}

	void ThrusterPartNode::load(rapidjson::Value& partData)
	{
		ResNode::load(partData);
	}

	void ThrusterPartNode::syncName()
	{
		char formatName[512];
		sprintf_s(formatName, 512, TRC(u8"ÍÆ½øÆ÷ %s"),m_part->getName().c_str());
		name = formatName;
	}

	std::string ThrusterPartNode::getResType()
	{
		return "Thruster";
	}
}
