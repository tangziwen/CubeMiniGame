#include "UseNode.h"
#include "CubeGame/ResNode.h"
#include "../BearPart.h"
#include "SpinNode.h"

namespace tzw
{
	UseNode::UseNode()
	{
		name =u8"使用";
		m_bearingAttr = addIn(u8"节点");
	}

	NodeAttrValue UseNode::execute()
	{
		auto attrVal = m_bearingAttr->eval();
		for(const auto& val : attrVal.m_list)
		{
			auto node = static_cast<ResNode *>(val.usrPtr);
			auto part = dynamic_cast<GamePart *>(node->getProxy());
			part->use();
		}
		return NodeAttrValue();
	}

	int UseNode::getNodeClass()
	{
		return Node_CLASS_USE;
	}
}
