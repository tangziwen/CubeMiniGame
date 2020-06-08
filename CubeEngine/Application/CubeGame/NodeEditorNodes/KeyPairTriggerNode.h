#pragma once
#include "../TriggerNode.h"

namespace tzw {

struct KeyPairTriggerNode: public TriggerNode 
{
public:
	KeyPairTriggerNode();
	virtual void trigger();
	void privateDraw() override;
	void handleKeyPress(int keyCode);
	void handleKeyRelease(int keyCode);
	int getNodeClass() override;
	void load(rapidjson::Value& partData) override;
	void dump(rapidjson::Value& partDocObj,
	                rapidjson::Document::AllocatorType& allocator) override;
protected:
	void triggerSignalChanged();
	bool isPlayerOnSeat();
	NodeAttr * m_onSignalChangedAttr;
	NodeAttr * m_signalAttr;
	int m_keyCodeA;
	int m_keyCodeB;
	int m_signal;
	bool m_isNeedOnSeat;
};

}
