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
class LinePrimitive;
class AssistDrawSystem : public Singleton<AssistDrawSystem>
{
public:
	AssistDrawSystem();
	void handleDraw(float dt);
	bool getIsShowAssistInfo();
	void setIsShowAssistInfo(bool newVal);
	LabelNew * getOrCreate(GamePart * part);
	void setShowAllPartLabel(bool isShow);
	void drawBoundingBox(AABB aabb, Matrix44 mat);
private:
	std::map<GamePart *, LabelNew *> m_partLabelMap;
	bool m_isShowAssistInfo;
	LinePrimitive * m_line;

};


}
