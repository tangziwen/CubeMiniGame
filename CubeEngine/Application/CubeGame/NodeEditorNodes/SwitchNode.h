#pragma once

#include "CubeGame/ResNode.h"

namespace tzw {
class SwitchPart;
struct SwitchNode: public ResNode 
{
public:
	SwitchNode(SwitchPart * part);
	virtual void trigger();
	void privateDraw() override;
	void handleKeyPress(int keyCode);
	void handleKeyRelease(int keyCode);
	int getNodeClass() override;
	void triggerPress();
	void syncName() override;
	void triggerRelease();
	GamePart* getProxy() override;
protected:

	bool isPlayerOnSeat();
	NodeAttr * m_onOn;
	NodeAttr * m_onOff;
	NodeAttr * m_stateAttr;
	SwitchPart * m_part;
};

}
