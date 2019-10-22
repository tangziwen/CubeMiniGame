#pragma once
#include "2D/GUISystem.h"
#include "CubeGame/ResNode.h"
namespace tzw {
class ThrusterPart;
struct ThrusterPartNode : public ResNode
{
public:
	ThrusterPartNode(ThrusterPart * canon);
	void privateDraw() override;
	virtual GamePart * getProxy();
	virtual void load(rapidjson::Value& partData);
	void syncName() override;
	std::string getResType() override;
private:
	ThrusterPart * m_part;
};

}
