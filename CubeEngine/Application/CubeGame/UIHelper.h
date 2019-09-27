#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"

namespace tzw
{
class LabelNew;

class UIHelper
{
TZW_SINGLETON_DECL(UIHelper)
public:
	UIHelper();
	void showFloatTips(std::string str, float duration = 1.0f, float disappearDuration = 0.5f);
};


}
