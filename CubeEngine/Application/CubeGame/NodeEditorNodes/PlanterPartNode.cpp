#include "PlanterPartNode.h"
#include "CubeGame/PlanterPart.h"
#include "Base/TranslationMgr.h"

namespace tzw
{
	PlanterPartNode::PlanterPartNode(PlanterPart * cannon)
	{
		m_part = cannon;
		PlanterPartNode::syncName();
	}

	void PlanterPartNode::privateDraw()
	{
		handleNameEdit();
	}

	GamePart* PlanterPartNode::getProxy()
	{
		return m_part;
	}

	void PlanterPartNode::load(rapidjson::Value& partData)
	{
		ResNode::load(partData);
	}

	void PlanterPartNode::syncName()
	{
		char formatName[512];
		sprintf_s(formatName, 512, TRC(u8"ÖÖÖ²»ú %s"),m_part->getName().c_str());
		name = formatName;
	}

	std::string PlanterPartNode::getResType()
	{
		return "CannonPart";
	}
}
