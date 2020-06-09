#pragma once
#include "2D/GUISystem.h"
#include "Base/GuidObj.h"
#include "rapidjson/document.h"

#define Node_TYPE_TRIGGER 0
#define Node_TYPE_RES 1
#define Node_TYPE_BEHAVIOR 2
#define Node_TYPE_OTHERS 3


#define Node_CLASS_WTF 0
#define Node_CLASS_KEY_TRIGGER 1
#define Node_CLASS_SPIN 2
#define Node_CLASS_VECTOR 3
#define Node_CLASS_USE 4
#define Node_CLASS_CONSTANT_INT 5
#define Node_CLASS_KEY_ANY_TRIGGER 6
#define Node_CLASS_TOGGLE 7
#define Node_CLASS_SWITCH 8
#define Node_CLASS_Button 9
#define Node_CLASS_VAR 10
#define Node_CLASS_ASSIGN 11
#define Node_CLASS_IF 12
#define Node_CLASS_EQUAL 13
#define Node_CLASS_PRINT 14
#define Node_CLASS_KEY_PAIR 15
#define NODE_CLASS_VOID 16
namespace tzw {
	struct GraphNodeInfo
	{
		int m_class;
		int m_type;
		std::string m_title;
		std::string m_desc;
	};

	class GraphNodeInfoMgr:public Singleton<GraphNodeInfoMgr>
	{
	public:
		GraphNodeInfoMgr();
		void load();
		GraphNodeInfo * get(int NodeClass);
		int amount();
	private:
		std::vector<GraphNodeInfo *> m_infoList;
	};
}
