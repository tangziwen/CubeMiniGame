#pragma once
#include "Engine/EngineDef.h"
#include "Base/Node.h"
#include "BlockPart.h"
#include <set>
#include "Island.h"
#include "Math/Ray.h"
#include "GameItem.h"
#include <map>
namespace tzw
{
class AssistDrawSystem;

class AssistDrawSystem
{
TZW_SINGLETON_DECL(AssistDrawSystem)
public:
	AssistDrawSystem();
	void handleDraw(float dt);
	bool getIsShowAssistInfo();
	void setIsShowAssistInfo(bool newVal);
	LabelNew * getOrCreate(GamePart * part);
	void setShowAllPartLabel(bool isShow);
private:
	std::map<GamePart *, LabelNew *> m_partLabelMap;
	bool m_isShowAssistInfo;

};


}
