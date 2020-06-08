#pragma once
#include "../TriggerNode.h"

namespace tzw {

struct KeyAnyTriggerNode: public TriggerNode 
{
public:
	KeyAnyTriggerNode();
	virtual void trigger();
	void privateDraw() override;
	void handleKeyPress(int keyCode);
	void handleKeyRelease(int keyCode);
	int getNodeClass() override;
	void load(rapidjson::Value& partData) override;
	void dump(rapidjson::Value& partDocObj,
	                rapidjson::Document::AllocatorType& allocator) override;
protected:
	void triggerPress();
	void triggerRelease();
	bool isPlayerOnSeat();
	NodeAttr * m_pressedAttr;
	NodeAttr * m_ReleasedAttr;
	int m_keyCode;
	bool m_isNeedOnSeat;
};

}
