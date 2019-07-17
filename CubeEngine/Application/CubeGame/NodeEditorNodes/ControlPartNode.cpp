#include "ControlPartNode.h"
#include "CubeGame/BearPart.h"
#include "BearingPartNode.h"
#include "CubeGame/ControlPart.h"

namespace tzw
{
	ControlPartNode::ControlPartNode(ControlPart * part)
	{
		m_part = part;
		char formatName[512];
		sprintf_s(formatName, 512, "Control %p",part);
		name = formatName;
		auto attr = addOut("Forward/Backward");
		attr->tag = 1;
		attr = addOut("Leftward/Rightward");
		attr->tag = 2;
	}

	void ControlPartNode::privateDraw()
	{
	}

	void ControlPartNode::onLinkOut(int startID, int endID, GameNodeEditorNode* other)
	{
		auto a = static_cast<BearingPartNode *>(other);
		auto attr = getAttrByGid(startID);
		if (attr->tag == 1) {
            m_part->addForwardBearing(a->getProxy());
		} else if(attr->tag == 2) 
		{
			m_part->addSidewardBearing(a->getProxy());
		}
	}
}
