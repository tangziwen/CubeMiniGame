#include "SpinNode.h"
#include "CubeGame/ResNode.h"
#include "../BearPart.h"

namespace tzw
{
	SpinNode::SpinNode()
	{
		name =u8"Ðý×ª";
		m_bearingAttr = addIn(u8"Öá³Ð");
		m_signalAttr = addIn(u8"´¥·¢ÐÅºÅ");
	}

	void SpinNode::execute(GameNodeEditorNode* from)
	{
		auto attrVal = m_bearingAttr->eval();

		auto node = static_cast<ResNode *>(attrVal.usrPtr);
		auto constraint = dynamic_cast<BearPart *>(node->getProxy());

		int signal = m_signalAttr->eval().int_val;
		if(constraint)
		{
			constraint->enableAngularMotor(true, 10.0f * signal, 50);
		}
	}
}
