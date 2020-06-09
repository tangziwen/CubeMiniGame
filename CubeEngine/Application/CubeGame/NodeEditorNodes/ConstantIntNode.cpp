#include "ConstantIntNode.h"
#include "CubeGame/CannonPart.h"


namespace tzw
{
	ConstantIntNode::ConstantIntNode()
	{
		name =TR(u8"����");
		m_attr = addOut(TR(u8"Self"));
		m_attr->m_localAttrValue.setInt(0);
	}

	void ConstantIntNode::privateDraw()
	{
		int intValue = m_attr->m_localAttrValue.getInt();
		ImGui::PushItemWidth(80);
		bool isInput = ImGui::InputInt("",&intValue);
		ImGui::PopItemWidth();
		if(isInput)
		{ 
			m_attr->m_localAttrValue.setInt(intValue);
		}
	}

	void ConstantIntNode::load(rapidjson::Value& partData)
	{
		GraphNode::load(partData);
		m_attr->m_localAttrValue.setInt(partData["INT_VALUE"].GetInt());
	}

	void ConstantIntNode::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		GraphNode::dump(partDocObj, allocator);
		partDocObj.AddMember("INT_VALUE", m_attr->m_localAttrValue.getInt(), allocator);
	}

	int ConstantIntNode::getNodeClass()
	{
		return Node_CLASS_CONSTANT_INT;
	}
}
