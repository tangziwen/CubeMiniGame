#include "ResNode.h"
#include "2D/imnodes.h"

namespace tzw
{
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
			bool isInputName = ImGui::InputText(u8"Ãû³Æ",a,128);
			if(isInputName)
			{
				getProxy()->setName(a);
				syncName();
			}
		}

	}
}
