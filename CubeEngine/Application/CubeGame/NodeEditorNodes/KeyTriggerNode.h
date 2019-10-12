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
	int m_forward;
	int m_side;
	NodeAttr * m_forwardAttr;
	NodeAttr * m_sideAttr;
	NodeAttr * m_forwardSignalAttr;
	NodeAttr * m_sideSignalAttr;
};

}
