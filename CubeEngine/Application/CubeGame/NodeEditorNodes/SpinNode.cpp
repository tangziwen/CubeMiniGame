#include "SpinNode.h"
#include "CubeGame/ResNode.h"
#include "../BearPart.h"

namespace tzw
{
	SpinNode::SpinNode()
	{
		name =TR(u8"旋转");
		m_bearingAttr = addIn(TR(u8"轴承"));
		m_signalAttr = addInSignal(TR(u8"方向信号"), 1);
		m_rotateSpeedAttr = addInFloat(TR(u8"转速"), 10.0f);
	}

	NodeAttrValue SpinNode::execute()
	{
		auto attrVal = m_bearingAttr->eval();
		for(auto val : attrVal.m_list)
		{
			auto node = static_cast<ResNode *>(val.usrPtr);
			auto constraint = dynamic_cast<BearPart *>(node->getProxy());

			int signal = m_signalAttr->eval().getInt();
			float speedResult = m_rotateSpeedAttr->eval().getNumber();
			if(constraint)
			{
				if(constraint->getIsSteering())
				{
					if(signal!= 0)
					{
						constraint->enableAngularMotor(true, speedResult * signal, 50);
					}else
					{
						constraint->enableAngularMotor(true, 0, 10000000.0f);
					}
				}else
				{
					if(signal!= 0)
					{
						constraint->enableAngularMotor(true, speedResult * signal, 50);
					}else
					{
						constraint->enableAngularMotor(false, speedResult, 50);
					}
				}
			}
		}
		return NodeAttrValue();
	}

	int SpinNode::getNodeClass()
	{
		return Node_CLASS_SPIN;
	}
}
