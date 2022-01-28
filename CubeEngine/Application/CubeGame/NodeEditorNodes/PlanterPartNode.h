#pragma once
#include "2D/GUISystem.h"
#include "CubeGame/ResNode.h"
namespace tzw {
class PlanterPart;
struct PlanterPartNode : public ResNode
{
public:
	PlanterPartNode(PlanterPart * canon);
	void privateDraw() override;
	virtual GamePart * getProxy();
	virtual void load(rapidjson::Value& partData);
	void syncName() override;
	std::string getResType() override;
private:
	PlanterPart * m_part;
};

}
