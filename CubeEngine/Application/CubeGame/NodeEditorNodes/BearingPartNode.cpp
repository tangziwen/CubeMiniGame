#include "BearingPartNode.h"
#include "CubeGame/BearPart.h"
#include "Base/TranslationMgr.h"

namespace tzw
{
	BearingPartNode::BearingPartNode(BearPart * bear)
	{
		m_part = bear;
		BearingPartNode::syncName();
	}

	void BearingPartNode::privateDraw()
	{
		handleNameEdit();

	}

	GamePart* BearingPartNode::getProxy()
	{
		return m_part;
	}

	void BearingPartNode::load(rapidjson::Value& partData)
	{
		ResNode::load(partData);
	}

	void BearingPartNode::syncName()
	{
		char formatName[512];
		sprintf_s(formatName, 512, TRC(u8"Öá³Ð %s"),m_part->getName().c_str());
		name = formatName;
	}

	std::string BearingPartNode::getResType()
	{
		return "BearPart";
	}
}
