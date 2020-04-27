#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"

namespace tzw
{
class LabelNew;

class UIHelper : public Singleton<UIHelper>
{
public:
	UIHelper();
	void showFloatTips(std::string str, float duration = 1.0f, float disappearDuration = 0.5f);
};


}
