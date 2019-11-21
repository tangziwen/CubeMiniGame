#include "VarNode.h"
#include "CubeGame/CannonPart.h"


namespace tzw
{
	VarNode::VarNode()
	{
		name =TR(u8"变量");
		m_attr = addOut(TR(u8"Self"));
		m_attr->m_localAttrValue.setInt(0);
		m_varName = "emptyFuckVar";
	}

	void VarNode::privateDraw()
	{
		int intValue = m_attr->m_localAttrValue.getInt();
		ImGui::PushItemWidth(80);
		char a[128] = "";
		strcpy(a, m_varName.c_str());
		bool isInputName = ImGui::InputText(TRC(u8"变量名"),a,128);
		bool isInput = ImGui::InputInt("",&intValue);
		ImGui::PopItemWidth();
		if(isInput)
		{ 
			m_attr->m_localAttrValue.setInt(intValue);
		}
		if(isInputName)
		{
			m_varName = a;
		}
	}

	void VarNode::load(rapidjson::Value& partData)
	{
		GameNodeEditorNode::load(partData);
		m_attr->m_localAttrValue.setInt(partData["INT_VALUE"].GetInt());
		m_varName = partData["VarName"].GetString();
	}

	void VarNode::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		GameNodeEditorNode::dump(partDocObj, allocator);
		partDocObj.AddMember("INT_VALUE", m_attr->m_localAttrValue.getInt(), allocator);
		partDocObj.AddMember("VarName", m_varName, allocator);
	}

	int VarNode::getNodeClass()
	{
		return Node_CLASS_VAR;
	}

	void VarNode::setInt(int value)
	{
		m_attr->m_localAttrValue.setInt(value);
	}

	void VarNode::setFloat(float value)
	{
		m_attr->m_localAttrValue.setInt(value);
	}

	int VarNode::getInt()
	{
		return m_attr->m_localAttrValue.getInt();
	}

	float VarNode::getFloat()
	{
		return m_attr->m_localAttrValue.getFloat();
	}

	std::string VarNode::getVarName()
	{
		return m_varName;
	}
}
