#include "GraphNode.h"
#include "2D/imnodes.h"
#include "GameUISystem.h"

namespace tzw
{
	static int g_attr_uid = 0;

	static void increaseAttrGID()
	{
		g_attr_uid += 1;
	}

	NodeAttrValuePrimitive::NodeAttrValuePrimitive(int val)
	{
		int_val = val;
		m_type = Type::INT;
	}

	NodeAttrValuePrimitive::NodeAttrValuePrimitive(float val)
	{
		float_val = val;
		m_type = Type::FLOAT;
	}

	NodeAttrValuePrimitive::NodeAttrValuePrimitive(void* val)
	{
		usrPtr = val;
		m_type = Type::USER_PTR;
	}

	NodeAttrValuePrimitive::NodeAttrValuePrimitive(std::string val)
	{
		m_strVal = val;
		m_type = Type::STRING;
	}

	NodeAttrValuePrimitive::NodeAttrValuePrimitive()
	{
		m_type = Type::VOID;
	}

	NodeAttrValue::NodeAttrValue()
	{
		
	}

	int NodeAttrValue::getInt()
	{
		return m_list[0].int_val;
	}

	float NodeAttrValue::getFloat()
	{
		return m_list[0].float_val;
	}

	std::string& NodeAttrValue::getStr()
	{
		return m_list[0].m_strVal;
	}

	void* NodeAttrValue::getUsrPtr()
	{
		return m_list[0].usrPtr;
	}

	float NodeAttrValue::getNumber()
	{
		switch(m_list[0].m_type)
		{
            case NodeAttrValuePrimitive::Type::FLOAT:
			return m_list[0].float_val;
            case NodeAttrValuePrimitive::Type::INT:
			return m_list[0].int_val;
            default:
			assert(0);
			break;
		}
	}

	void NodeAttrValue::setInt(int value)
	{
		m_list.clear();
		m_list.push_back(NodeAttrValuePrimitive(value));
	}

	void NodeAttrValue::setFloat(float value)
	{
		m_list.clear();
		m_list.push_back(NodeAttrValuePrimitive(value));
	}

	void NodeAttrValue::setUsrPtr(void* value)
	{
		m_list.clear();
		m_list.push_back(NodeAttrValuePrimitive(value));
	}

	void NodeAttrValue::setString(std::string newStr)
	{
		m_list.clear();
		m_list.push_back(NodeAttrValuePrimitive(newStr));
	}

	NodeAttrValuePrimitive NodeAttrValue::getFirst()
	{
		return m_list[0];
	}

	NodeAttr::NodeAttr()
	{
		dataType = DataType::DATA;
		m_localAttrValue = NodeAttrValue();
		acceptValueType = AcceptValueType::ANY;
	}
	//ÇóÓÒÖµ
	NodeAttrValue NodeAttr::eval()
	{
		auto nodeEditor = GameUISystem::shared()->getNodeEditor();
		
		if(this->dataType ==DataType::RETURN_VALUE)//is a return value, must execute
		{
			//all pass
			return (m_parent)->execute();
		}
		else
		{
			auto attr = nodeEditor->findAttrLinksFromAttr(this);
			if(attr)
			{
				return attr->eval();
			}
			else 
			{
				return m_localAttrValue;
			}
		}

	}

	NodeAttr* NodeAttr::evalRef()
	{
		auto nodeEditor = GameUISystem::shared()->getNodeEditor();
		
		if(this->dataType ==DataType::RETURN_VALUE)//is a return value, must execute
		{
			//all pass
			return this;
		}
		else
		{
			auto attr = nodeEditor->findAttrLinksFromAttr(this);
			if(attr)
			{
				return attr->evalRef();
			}
			else 
			{
				return this;
			}
		}
	}

	NodeAttr* GraphNode::addIn(std::string attrName)
	{
		increaseAttrGID();
		auto attr = new NodeAttr();
		attr->m_name = attrName;
		attr->gID = g_attr_uid;
		attr->type = NodeAttr::Type::INPUT_ATTR;
		attr->m_parent = this;
		m_inAttr.push_back(attr);
		return attr;
	}

	NodeAttr* GraphNode::addInExe(std::string attrName)
	{
		auto node = addIn(attrName);
		node->dataType = NodeAttr::DataType::EXECUTE;
		return node;
	}

	NodeAttr* GraphNode::addOut(std::string attrName)
	{
		increaseAttrGID();
		auto attr = new NodeAttr();
		attr->m_name = attrName;
		attr->gID = g_attr_uid;
		attr->type = NodeAttr::Type::OUTPUT_ATTR;
		m_outAttr.push_back(attr);
		attr->m_parent = this;
		return attr;
	}

	NodeAttr* GraphNode::addOutExe(std::string attrName)
	{
		auto node = addOut(attrName);
		node->dataType = NodeAttr::DataType::EXECUTE;
		return node;
	}

	NodeAttr* GraphNode::addOutReturn()
	{
		auto node = addOut("Return");
		node->dataType = NodeAttr::DataType::RETURN_VALUE;
		return node;
	}

	NodeAttr* GraphNode::addInInt(std::string attrName, int defaultValue)
	{
		auto node = addIn(attrName);
		node->acceptValueType = NodeAttr::AcceptValueType::INT;
		node->m_localAttrValue.setInt(defaultValue);
		return node;
	}

	NodeAttr* GraphNode::addInSignal(std::string attrName, int defaultValue)
	{
		auto node = addIn(attrName);
		node->acceptValueType = NodeAttr::AcceptValueType::SIGNAL;
		node->m_localAttrValue.setInt(defaultValue);
		return node;
	}

	NodeAttr* GraphNode::addInFloat(std::string attrName, float defaultValue)
	{
		auto node = addIn(attrName);
		node->acceptValueType = NodeAttr::AcceptValueType::FLOAT;
		node->m_localAttrValue.setFloat(defaultValue);
		return node;
	}

	NodeAttr* GraphNode::addInStr(std::string attrName, std::string defaultValue)
	{
		auto node = addIn(attrName);
		node->acceptValueType = NodeAttr::AcceptValueType::STRING;
		node->m_localAttrValue.setString(defaultValue);
		return node;
	}

	GraphNode::GraphNode()
	{
		increaseAttrGID();
		m_nodeID = g_attr_uid;
		isShowed = false;
		m_nodeInfo = GraphNodeInfoMgr::shared()->get(0);
	}

	std::vector<NodeAttr*>& GraphNode::getInAttrs()
	{
		return m_inAttr;
	}

	std::vector<NodeAttr*>& GraphNode::getOuAttrs()
	{
		return m_outAttr;
	}

	void GraphNode::privateDraw()
	{

	}

	NodeAttr* GraphNode::checkInNodeAttr(int gid)
	{
		for(auto attr: m_inAttr) 
		{
			if (attr->gID == gid) return attr;  
		}
		return nullptr;
	}

	NodeAttr* GraphNode::checkOutNodeAttr(int gid)
	{
		for(auto attr: m_outAttr) 
		{
			if (attr->gID == gid) return attr;  
		}
		return nullptr;
	}

	void GraphNode::onLinkOut(int startID, int endID, GraphNode* other)
	{
	}

	void GraphNode::onRemoveLinkOut(int startID, int endID, GraphNode* other)
	{
	}

	void GraphNode::onRemoveLinkIn(int startID, int endID, GraphNode* other)
	{
	}

	void GraphNode::onLinkIn(int startID, int endID, GraphNode* other)
	{
	}

	NodeAttr* GraphNode::getAttrByGid(int GID)
	{
		
		for(auto attr : m_inAttr) {
			if(attr->gID == GID)
				return attr;
		}

		for(auto attr : m_outAttr) {
			if(attr->gID == GID)
				return attr;
		}
		return nullptr;
	}

	GraphNode::~GraphNode()
	{
		tlog("remove node\n");
	}

	int GraphNode::getInputAttrLocalIndexByGid(int GID)
	{
		for(int i = 0; i < m_inAttr.size(); i++) {
			if(m_inAttr[i]->gID == GID)
				return i;
		}
		return -1;
	}

	int GraphNode::getOutputAttrLocalIndexByGid(int GID)
	{
		for(int i = 0; i < m_outAttr.size(); i++) {
			if(m_outAttr[i]->gID == GID)
				return i;
		}
		return -1;
	}

	NodeAttr* GraphNode::getInByIndex(int localIndex)
	{
		return m_inAttr[localIndex];
	}

	NodeAttr* GraphNode::getOutByIndex(int localIndex)
	{
		return m_outAttr[localIndex];
	}

	NodeAttr* GraphNode::getInByGid(int globalIndex)
	{
		auto index = getInputAttrLocalIndexByGid(globalIndex);
		if( index>= 0)
		{
			return m_inAttr[index];
		}
		return nullptr;
	}

	NodeAttr* GraphNode::getOutByGid(int globalIndex)
	{
		auto index = getOutputAttrLocalIndexByGid(globalIndex);
		if( index>= 0)
		{
			return m_outAttr[index];
		}
		return nullptr;
	}

	vec3 GraphNode::getNodeColor()
	{
		return vec3(1, 1, 1);
	}

	void GraphNode::load(rapidjson::Value& partData)
	{
		//load Node Origin from file
		m_origin = vec2(partData["orgin_x"].GetDouble(), partData["orgin_y"].GetDouble());
		this->setGUID(partData["UID"].GetString());
		//load inAttr local Value
		for(int i = 0; i < m_inAttr.size(); i++)
		{
			auto inAttr = m_inAttr[i];
			switch(inAttr->acceptValueType )
			{
              case NodeAttr::AcceptValueType::ANY:
				{
					//no local value at all
				}
				break;
              case NodeAttr::AcceptValueType::INT:
				{
              		inAttr->m_localAttrValue.setInt(partData["InAttrList"][i]["LocalValue"].GetInt());
				}
				break;
              case NodeAttr::AcceptValueType::FLOAT:
				{
              		inAttr->m_localAttrValue.setFloat(partData["InAttrList"][i]["LocalValue"].GetDouble());
				}
				break;
              case NodeAttr::AcceptValueType::SIGNAL:
				{
              		inAttr->m_localAttrValue.setInt(partData["InAttrList"][i]["LocalValue"].GetInt());
				}
				break;
              case NodeAttr::AcceptValueType::STRING:
				{
					inAttr->m_localAttrValue.setString(partData["InAttrList"][i]["LocalValue"].GetString());
				}
				break;
			}
			
		}
	}

	void GraphNode::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		partDocObj.AddMember("UID", std::string(getGUID()), allocator);
		partDocObj.AddMember("NodeType", getType(), allocator);
		partDocObj.AddMember("NodeClass", getNodeClass(), allocator);
		rapidjson::Value inAttrListObj(rapidjson::kArrayType);

		//Dump Node attr IN
		for(auto inAttr :m_inAttr)
		{
			rapidjson::Value attrObj(rapidjson::kObjectType);
			switch(inAttr->acceptValueType )
			{
              case NodeAttr::AcceptValueType::ANY:
				{
					attrObj.AddMember("LocalValue", "ANY",allocator);
				}
				break;
              case NodeAttr::AcceptValueType::INT:
				{
					attrObj.AddMember("LocalValue", inAttr->m_localAttrValue.getInt(),allocator);
				}
				break;
              case NodeAttr::AcceptValueType::FLOAT:
				{
					attrObj.AddMember("LocalValue", inAttr->m_localAttrValue.getFloat(),allocator);
				}
				break;
              case NodeAttr::AcceptValueType::SIGNAL:
				{
					attrObj.AddMember("LocalValue", inAttr->m_localAttrValue.getInt(),allocator);
				}
				break;
              case NodeAttr::AcceptValueType::STRING:
				{
					attrObj.AddMember("LocalValue", inAttr->m_localAttrValue.getStr(),allocator);
				}
				break;
			}
			inAttrListObj.PushBack(attrObj, allocator);
		}
		partDocObj.AddMember("InAttrList",inAttrListObj, allocator);
	}

	NodeAttrValue GraphNode::execute()
	{
		return NodeAttrValue();
	}

	void GraphNode::handleExeOut()
	{
		
	}

	int GraphNode::getType()
	{
		if(m_nodeInfo)
		{
			return m_nodeInfo->m_type;
		}
		return Node_TYPE_OTHERS;
	}

	int GraphNode::getNodeClass()
	{
		if(m_nodeInfo)
		{
			return m_nodeInfo->m_class;
		}
		return Node_CLASS_WTF;
	}

	void GraphNode::loadInfo(int classID)
	{
		m_nodeInfo = GraphNodeInfoMgr::shared()->get(classID);
	}
}
