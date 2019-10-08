#include "ResNode.h"


namespace tzw
{
	ResNode::ResNode()
	{

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
			bool isInputName = ImGui::InputText(u8"Ãû³Æ",a,128);
			ImGui::PopItemWidth();
			if(isInputName)
			{
				getProxy()->setName(a);
				syncName();
			}
		}

	}
}
