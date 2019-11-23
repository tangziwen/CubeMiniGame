#include "PrintNode.h"
#include "CubeGame/ResNode.h"
#include "../BearPart.h"
#include "VarNode.h"
#include "CubeGame/UIHelper.h"

namespace tzw
{
	PrintNode::PrintNode()
	{
		name =TR(u8"´òÓ¡");
		m_strAttrVal = addInStr(TR(u8"×Ö·û´®"), "let me do that shit");
	}

	NodeAttrValue PrintNode::execute()
	{
		UIHelper::shared()->showFloatTips(m_strAttrVal->eval().getStr());
		return NodeAttrValue();
	}


	int PrintNode::getNodeClass()
	{
		return Node_CLASS_PRINT;
	}
}
