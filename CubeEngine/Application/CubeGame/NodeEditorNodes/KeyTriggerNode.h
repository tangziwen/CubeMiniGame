#pragma once
#include "../TriggerNode.h"

namespace tzw {

struct KeyTriggerNode: public TriggerNode 
{
public:
	KeyTriggerNode();
	virtual void trigger();
	int getType() override;
	void handleKeyPress(int keyCode);
	void handleKeyRelease(int keyCode);
protected:
	int m_forward;
	int m_side;
	NodeAttr * m_forwardAttr;
	NodeAttr * m_sideAttr;
	NodeAttr * m_forwardSignalAttr;
	NodeAttr * m_sideSignalAttr;
};

}
