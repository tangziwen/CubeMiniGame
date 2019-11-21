#include "GameNodeEditorNode.h"
#include "2D/imnodes.h"
#include "MainMenu.h"

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

	void* NodeAttrValue::getUsrPtr()
	{
		return m_list[0].usrPtr;
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
		auto nodeEditor = MainMenu::shared()->getNodeEditor();
		
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
		auto nodeEditor = MainMenu::shared()->getNodeEditor();
		
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

	NodeAttr* GameNodeEditorNode::addIn(std::string attrName)
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

	NodeAttr* GameNodeEditorNode::addInExe(std::string attrName)
	{
		auto node = addIn(attrName);
		node->dataType = NodeAttr::DataType::EXECUTE;
		return node;
	}

	NodeAttr* GameNodeEditorNode::addOut(std::string attrName)
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

	NodeAttr* GameNodeEditorNode::addOutExe(std::string attrName)
	{
		auto node = addOut(attrName);
		node->dataType = NodeAttr::DataType::EXECUTE;
		return node;
	}

	NodeAttr* GameNodeEditorNode::addOutReturn()
	{
		auto node = addOut("Return");
		node->dataType = NodeAttr::DataType::RETURN_VALUE;
		return node;
	}

	NodeAttr* GameNodeEditorNode::addInInt(std::string attrName, int defaultValue)
	{
		auto node = addIn(attrName);
		node->acceptValueType = NodeAttr::AcceptValueType::INT;
		node->m_localAttrValue.setInt(defaultValue);
		return node;
	}

	NodeAttr* GameNodeEditorNode::addInSignal(std::string attrName, int defaultValue)
	{
		auto node = addIn(attrName);
		node->acceptValueType = NodeAttr::AcceptValueType::SIGNAL;
		node->m_localAttrValue.setInt(defaultValue);
		return node;
	}

	NodeAttr* GameNodeEditorNode::addInFloat(std::string attrName, float defaultValue)
	{
		auto node = addIn(attrName);
		node->acceptValueType = NodeAttr::AcceptValueType::FLOAT;
		node->m_localAttrValue.setFloat(defaultValue);
		return node;
	}

	GameNodeEditorNode::GameNodeEditorNode()
	{
		increaseAttrGID();
		m_nodeID = g_attr_uid;
		isShowed = false;
	}

	std::vector<NodeAttr*>& GameNodeEditorNode::getInAttrs()
	{
		return m_inAttr;
	}

	std::vector<NodeAttr*>& GameNodeEditorNode::getOuAttrs()
	{
		return m_outAttr;
	}

	void GameNodeEditorNode::privateDraw()
	{

	}

	NodeAttr* GameNodeEditorNode::checkInNodeAttr(int gid)
	{
		for(auto attr: m_inAttr) 
		{
			if (attr->gID == gid) return attr;  
		}
		return nullptr;
	}

	NodeAttr* GameNodeEditorNode::checkOutNodeAttr(int gid)
	{
		for(auto attr: m_outAttr) 
		{
			if (attr->gID == gid) return attr;  
		}
		return nullptr;
	}

	void GameNodeEditorNode::onLinkOut(int startID, int endID, GameNodeEditorNode* other)
	{
	}

	void GameNodeEditorNode::onRemoveLinkOut(int startID, int endID, GameNodeEditorNode* other)
	{
	}

	void GameNodeEditorNode::onRemoveLinkIn(int startID, int endID, GameNodeEditorNode* other)
	{
	}

	void GameNodeEditorNode::onLinkIn(int startID, int endID, GameNodeEditorNode* other)
	{
	}

	NodeAttr* GameNodeEditorNode::getAttrByGid(int GID)
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

	GameNodeEditorNode::~GameNodeEditorNode()
	{
		tlog("remove node\n");
	}

	int GameNodeEditorNode::getInputAttrLocalIndexByGid(int GID)
	{
		for(int i = 0; i < m_inAttr.size(); i++) {
			if(m_inAttr[i]->gID == GID)
				return i;
		}
		return -1;
	}

	int GameNodeEditorNode::getOutputAttrLocalIndexByGid(int GID)
	{
		for(int i = 0; i < m_outAttr.size(); i++) {
			if(m_outAttr[i]->gID == GID)
				return i;
		}
		return -1;
	}

	NodeAttr* GameNodeEditorNode::getInByIndex(int localIndex)
	{
		return m_inAttr[localIndex];
	}

	NodeAttr* GameNodeEditorNode::getOutByIndex(int localIndex)
	{
		return m_outAttr[localIndex];
	}

	NodeAttr* GameNodeEditorNode::getInByGid(int globalIndex)
	{
		auto index = getInputAttrLocalIndexByGid(globalIndex);
		if( index>= 0)
		{
			return m_inAttr[index];
		}
		return nullptr;
	}

	NodeAttr* GameNodeEditorNode::getOutByGid(int globalIndex)
	{
		auto index = getOutputAttrLocalIndexByGid(globalIndex);
		if( index>= 0)
		{
			return m_outAttr[index];
		}
		return nullptr;
	}

	vec3 GameNodeEditorNode::getNodeColor()
	{
		return vec3(1, 1, 1);
	}

	void GameNodeEditorNode::load(rapidjson::Value& partData)
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
			}
			
		}
	}

	void GameNodeEditorNode::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
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
			}
			inAttrListObj.PushBack(attrObj, allocator);
		}
		partDocObj.AddMember("InAttrList",inAttrListObj, allocator);
	}

	NodeAttrValue GameNodeEditorNode::execute()
	{
		return NodeAttrValue();
	}

	void GameNodeEditorNode::handleExeOut()
	{
		
	}

	int GameNodeEditorNode::getType()
	{
		return Node_TYPE_OTHERS;
	}

	int GameNodeEditorNode::getNodeClass()
	{
		return Node_CLASS_WTF;
	}
}
