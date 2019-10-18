#pragma once
#include "../TriggerNode.h"

namespace tzw {

struct KeyTriggerNode: public TriggerNode 
{
public:
	KeyTriggerNode();
	virtual void trigger();
	void handleKeyPress(int keyCode);
	void handleKeyRelease(int keyCode);
	int getNodeClass() override;
protected:
	void triggerForward();
	void triggerSide();
	void triggerZ();
	void triggerX();
	void triggerC();
	bool isPlayerOnSeat();
	int m_forward;
	int m_side;
	NodeAttr * m_forwardAttr;
	NodeAttr * m_sideAttr;
	NodeAttr * m_forwardSignalAttr;
	NodeAttr * m_sideSignalAttr;

	NodeAttr * m_zKeyAttr;
	NodeAttr * m_xKeyAttr;
	NodeAttr * m_cKeyAttr;
};

}
