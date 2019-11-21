#include "EqualNode.h"
#include "CubeGame/ResNode.h"
#include "../BearPart.h"
#include "VarNode.h"

namespace tzw
{
	EqualNode::EqualNode()
	{
		name =TR(u8"等于");
		m_leftValAttr = addIn(TR(u8"左值"));
		m_rightValAttr = addInInt(TR(u8"右值"), 1);
		m_out = addOut("Return");
	}

	NodeAttrValue EqualNode::execute()
	{
		auto valueA = m_leftValAttr->eval();
		auto valueB = m_rightValAttr->eval();
		if(valueA.getInt() == valueB.getInt())
		{
			m_out->m_localAttrValue.setInt(1);
		}else
		{
			m_out->m_localAttrValue.setInt(0);
		}
		
		return m_out->m_localAttrValue;
	}


	int EqualNode::getNodeClass()
	{
		return Node_CLASS_EQUAL;
	}
}
