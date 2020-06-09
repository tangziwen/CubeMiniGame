#pragma once
#include "2D/GUISystem.h"
#include "Base/GuidObj.h"
#include "rapidjson/document.h"

#define Node_TYPE_TRIGGER 0
#define Node_TYPE_RES 1
#define Node_TYPE_BEHAVIOR 2
#define Node_TYPE_OTHERS 3


#include "GraphNodeInfo.h"
namespace tzw {
struct GraphNode;

struct NodeAttrValuePrimitive
{
	enum class Type
	{
		VOID,
	    INT,
	    FLOAT,
  		USER_PTR,
		STRING
	};
	NodeAttrValuePrimitive(int val);
	NodeAttrValuePrimitive(float val);
	NodeAttrValuePrimitive(void * val);
	NodeAttrValuePrimitive(std::string val);
	NodeAttrValuePrimitive();
	Type m_type;
	int int_val;
	float float_val;
	void * usrPtr;
	std::string m_strVal;
};
struct NodeAttrValue
{
	NodeAttrValue();
	int getInt();
	float getFloat();
	std::string& getStr();
	void * getUsrPtr();
	float getNumber();
	void setInt(int value);
	void setFloat(float value);
	void setUsrPtr(void * value);
	void setString(std::string newStr);
	NodeAttrValuePrimitive getFirst();
	std::vector<NodeAttrValuePrimitive> m_list;
};
	
struct NodeAttr
{
	enum class Type
	{
	INPUT_ATTR,
	OUTPUT_ATTR,
	INVALID
	};
	enum class DataType
	{
		EXECUTE,
		DATA,
		RETURN_VALUE,
		DATA_WITH_DEFAULT_VALUE,
	};
	enum class AcceptValueType //可接受的valueType, int和float类型允许使用默认值
	{
		ANY,
		INT,
		FLOAT,
		Res,
		SIGNAL, //signal is the same like int ,but only allow for -1, 0, 1 as value
		STRING
	};
	std::string m_name;
	int gID;
	int tag;
	Type type;
	DataType dataType;
	AcceptValueType acceptValueType;
	NodeAttr();
	NodeAttrValue eval();
	NodeAttr* evalRef();
	NodeAttrValue m_localAttrValue{};
	GraphNode * m_parent;
};

struct GraphNode : public GuidObj
{
public:
	NodeAttr* addIn(std::string attrName);
	NodeAttr* addInExe(std::string attrName);
	NodeAttr* addOut(std::string attrName);
	NodeAttr* addOutExe(std::string attrName);
	NodeAttr * addOutReturn();
	NodeAttr* addInInt(std::string attrName, int defaultValue);
	NodeAttr* addInSignal(std::string attrName, int defaultValue);
	NodeAttr* addInFloat(std::string attrName, float defaultValue);
	NodeAttr* addInStr(std::string attrName, std::string defaultValue);
	std::string name;
	GraphNode();
	std::vector<NodeAttr*>& getInAttrs();
	std::vector<NodeAttr*>& getOuAttrs();
	virtual void privateDraw();
	NodeAttr* checkInNodeAttr(int gid);
	NodeAttr* checkOutNodeAttr(int gid);
	virtual void onLinkOut(int startID, int endID, GraphNode* other);
	virtual void onRemoveLinkOut(int startID,
	                           int endID,
	                           GraphNode* other);
	virtual void onRemoveLinkIn(int startID,
	                          int endID,
	                          GraphNode* other);
	virtual void onLinkIn(int startID, int endID, GraphNode* other);
	NodeAttr* getAttrByGid(int GID);
	virtual ~GraphNode();
	int getInputAttrLocalIndexByGid(int GID);
	int getOutputAttrLocalIndexByGid(int GID);
	NodeAttr* getInByIndex(int localIndex);
	NodeAttr* getOutByIndex(int localIndex);
	NodeAttr* getInByGid(int GID);
	NodeAttr* getOutByGid(int GID);
	virtual vec3 getNodeColor();
	virtual void load(rapidjson::Value& partData);
	virtual void dump(rapidjson::Value& partDocObj,
	                rapidjson::Document::AllocatorType& allocator);
	virtual NodeAttrValue execute();
	virtual void handleExeOut();
	int m_nodeID;
	vec2 m_origin;
	bool isShowed;
	virtual int getType();
	virtual int getNodeClass();
	void loadInfo(int classID);
protected:
	std::vector<NodeAttr*> m_inAttr;
	std::vector<NodeAttr*> m_outAttr;
	std::map<int, int> m_inGlobalMap;
	std::map<int, int> m_OutGlobalMap;
	GraphNodeInfo * m_nodeInfo;
};
}
