#pragma once
#include "2D/GUISystem.h"
#include "CubeGame/ResNode.h"
namespace tzw {
class ButtonPart;
struct ButtonPartNode : public ResNode
{
public:
	ButtonPartNode(ButtonPart * canon);
	void privateDraw() override;
	GamePart * getProxy() override;
	virtual void load(rapidjson::Value& partData);
	void syncName() override;
	void triggerPress();
	void triggerRelease();
	std::string getResType() override;
	int getNodeClass() override;
private:
	ButtonPart * m_part;
	NodeAttr * m_pressedAttr;
	NodeAttr * m_releasedAttr;
	NodeAttr * m_stateAttr;
};

}
