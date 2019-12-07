#include "VectorNode.h"
#include "CubeGame/ResNode.h"
#include "CubeGame/MainMenu.h"

namespace tzw
{
	VectorNode::VectorNode()
	{
		name =TR(u8"ÏòÁ¿");
		m_composite = addIn(TR(u8"¾ÛºÏ"));
		m_out = addOut("Result");
	}

	NodeAttrValue VectorNode::execute()
	{
		auto nodeEditor = MainMenu::shared()->getNodeEditor();
		auto theList = nodeEditor->findAllAttrLinksFromAttr(m_composite);
		auto& value = m_out->m_localAttrValue;
		value.m_list.clear();
		for(auto attr : theList)
		{
			value.m_list.push_back(attr->eval().getFirst());
		}
		return value;
	}

	int VectorNode::getNodeClass()
	{
		return Node_CLASS_VECTOR;
	}
}
