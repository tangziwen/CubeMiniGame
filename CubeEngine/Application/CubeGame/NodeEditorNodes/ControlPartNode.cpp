#include "ControlPartNode.h"
#include "CubeGame/BearPart.h"
#include "BearingPartNode.h"
#include "CubeGame/ControlPart.h"

namespace tzw
{
	ControlPartNode::ControlPartNode(ControlPart * part)
	{
		m_part = part;
		ControlPartNode::syncName();
		auto attr = addOut(u8"前进/后退");
		attr->tag = 1;
		attr = addOut(u8"向左/向右");
		attr->tag = 2;
	}

	void ControlPartNode::privateDraw()
	{
		handleNameEdit();
	}

	void ControlPartNode::onLinkOut(int startID, int endID, GameNodeEditorNode* other)
	{
		auto a = static_cast<BearingPartNode *>(other);
		auto attr = getAttrByGid(startID);
		if (attr->tag == 1) {
            m_part->addForwardBearing(reinterpret_cast<BearPart*>(a->getProxy()));
		} else if(attr->tag == 2) 
		{
			m_part->addSidewardBearing(reinterpret_cast<BearPart*>(a->getProxy()));
		}
	}

	void ControlPartNode::load(rapidjson::Value& partData)
	{
	}

	void ControlPartNode::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		partDocObj.AddMember("Type", std::string("Resource"), allocator);
		partDocObj.AddMember("ResType", std::string("ControlPart"), allocator);
		partDocObj.AddMember("ResUID", std::string(m_part->getGUID()), allocator);
		partDocObj.AddMember("UID", std::string(getGUID()), allocator);
	}

	void ControlPartNode::syncName()
	{
		char formatName[512];
		sprintf_s(formatName, 512, u8"控制模块 %s",m_part->getName().c_str());
		name = formatName;
	}

	GamePart* ControlPartNode::getProxy()
	{
		return m_part;
	}
}
