#pragma once
#include "../TriggerNode.h"

namespace tzw {

struct SwitchNode: public TriggerNode 
{
public:
	SwitchNode();
	virtual void trigger();
	void privateDraw() override;
	void handleKeyPress(int keyCode);
	void handleKeyRelease(int keyCode);
	int getNodeClass() override;
protected:
	void triggerPress();
	void triggerRelease();
	bool isPlayerOnSeat();
	NodeAttr * m_pressedAttr;
	NodeAttr * m_ReleasedAttr;
	int m_keyCode;
};

}
