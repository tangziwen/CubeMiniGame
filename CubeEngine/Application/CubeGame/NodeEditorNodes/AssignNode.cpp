#include "AssignNode.h"
#include "CubeGame/ResNode.h"
#include "../BearPart.h"
#include "VarNode.h"

namespace tzw
{
	AssignNode::AssignNode()
	{
		name =TR(u8"赋值");
		m_leftValAttr = addIn(TR(u8"左值"));
		m_rightValAttr = addIn(TR(u8"右值"));
	}

	NodeAttrValue AssignNode::execute()
	{
		auto attrRef = m_leftValAttr->evalRef();
		auto value = m_rightValAttr->eval();
		auto var = static_cast<VarNode *>(attrRef->m_parent);
		tlog("the var %s has been set to %d by %s", var->getVarName().c_str(),value.getInt(), m_rightValAttr->evalRef()->m_parent->name.c_str());
		attrRef->m_localAttrValue = value;
		return NodeAttrValue();
	}

	int AssignNode::getNodeClass()
	{
		return Node_CLASS_ASSIGN;
	}
}
