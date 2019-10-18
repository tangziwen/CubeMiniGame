#pragma once
#include "2D/GUISystem.h"
#include "CubeGame/ResNode.h"
namespace tzw {
class CannonPart;
struct CannonPartNode : public ResNode
{
public:
	CannonPartNode(CannonPart * canon);
	void privateDraw() override;
	virtual GamePart * getProxy();
	virtual void load(rapidjson::Value& partData);
	void syncName() override;
	std::string getResType() override;
private:
	CannonPart * m_part;
};

}
