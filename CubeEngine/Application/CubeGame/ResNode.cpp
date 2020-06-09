#include "ResNode.h"


namespace tzw
{
	ResNode::ResNode()
	{
		auto atr = addOut(u8"Self");
		atr->m_localAttrValue.setUsrPtr(this);
	}

	void ResNode::syncName()
	{

	}

	GamePart* ResNode::getProxy()
	{
		return nullptr;
	}

	
	void ResNode::handleNameEdit()
	{
		if(getProxy())
		{
			char a[128] = "";
			strcpy(a, getProxy()->getName().c_str());
			ImGui::PushItemWidth(80);
			bool isInputName = ImGui::InputText(TRC(u8"Ãû³Æ"),a,128);
			ImGui::PopItemWidth();
			if(isInputName)
			{
				getProxy()->setName(a);
				syncName();
			}
		}

	}

	vec3 ResNode::getNodeColor()
	{
		return vec3(95.0f/ 255.0f, 127.0f/255.0f, 90.0f / 255.0f);
	}

	int ResNode::getType()
	{
		return Node_TYPE_RES;
	}

	std::string ResNode::getResType()
	{
		return "baseRes";
	}

	void ResNode::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		GraphNode::dump(partDocObj, allocator);
		partDocObj.AddMember("Type", std::string("Resource"), allocator);
		partDocObj.AddMember("ResType", getResType(), allocator);
		partDocObj.AddMember("ResUID", std::string(getProxy()->getGUID()), allocator);
		partDocObj.AddMember("UID", std::string(getGUID()), allocator);
		
	}
}
