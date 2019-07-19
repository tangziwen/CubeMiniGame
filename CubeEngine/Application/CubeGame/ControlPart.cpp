#include "ControlPart.h"
#include "3D/Primitive/CubePrimitive.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"
#include "Island.h"
#include "MainMenu.h"
#include "NodeEditorNodes/ControlPartNode.h"
#include "Event/EventMgr.h"
#include "GameWorld.h"

namespace tzw
{
const float blockSize = 0.5;
ControlPart::ControlPart()
{
	
	m_forward = 0;
	m_side = 0;
	m_isActivate = false;
	m_node = new CubePrimitive(blockSize, blockSize, blockSize);
	m_shape = new PhysicsShape();
	auto texture =  TextureMgr::shared()->getByPath("Texture/trestraou2_Base_Color.jpg");
	m_node->getMaterial()->setTex("diffuseMap", texture);
	m_shape->initBoxShape(vec3(blockSize, blockSize, blockSize));
	m_parent = nullptr;
	for(int i = 0; i < 6; i++)
	{
		m_bearPart[i] = nullptr;
	}
	initAttachments();
	auto nodeEditor = MainMenu::shared()->getNodeEditor();
	auto node = new ControlPartNode(this);
	nodeEditor->addNode(node);
	EventMgr::shared()->addFixedPiorityListener(this);
}

Attachment * ControlPart::findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & attachUp)
{
	RayAABBSide side;
	vec3 hitPoint;
	auto isHit = ray.intersectAABB(m_node->localAABB(), &side, hitPoint);

	if (!isHit) return nullptr;


	auto m = m_node->getLocalTransform().data();
	vec3 up(m[4], m[5], m[6]);
	vec3 forward(-m[8], -m[9], -m[10]);
	vec3 right(m[0], m[1], m[2]);
	up.normalize();
	forward.normalize();
	right.normalize();
	Attachment * attachPtr = nullptr;
	switch (side)
	{
	case RayAABBSide::up:
	{
		attachPtr = getAttachmentInfo(4, attachPosition, Normal, attachUp);
	}
	break;
	case RayAABBSide::down:
	{
		attachPtr = getAttachmentInfo(5, attachPosition, Normal, attachUp);
	}
	break;
	case RayAABBSide::left:
	{
		attachPtr = getAttachmentInfo(3, attachPosition, Normal, attachUp);
	}
	break;
	case RayAABBSide::right:
	{
		attachPtr = getAttachmentInfo(2, attachPosition, Normal, attachUp);
	}
	break;
	case RayAABBSide::front:
	{
		attachPtr = getAttachmentInfo(0, attachPosition, Normal, attachUp);
	}
	break;
	case RayAABBSide::back:
	{
		attachPtr = getAttachmentInfo(1, attachPosition, Normal, attachUp);
	}
	break;
	default:
		break;
	}
	return attachPtr;
}

void ControlPart::attachToFromOtherIsland(Attachment * attach, BearPart * bearing)
{
	auto islandMatrixInverted = m_parent->m_node->getLocalTransform().inverted();
	vec3 attachPosition,  Normal,  up;
	attach->getAttachmentInfoWorld(attachPosition, Normal, up);
	//transform other island attachment to our island
	attachPosition = islandMatrixInverted.transformVec3(attachPosition);
	Normal = islandMatrixInverted.transofrmVec4(vec4(Normal, 0.0)).toVec3();
	vec3 InvertedNormal = Normal * -1;
	attachPosition = attachPosition + Normal * 0.5;
	up = islandMatrixInverted.transofrmVec4(vec4(up, 0.0)).toVec3();
	//we use m_attachment[0]
	auto selfAttah = m_attachment[0];
	bearing->m_a = selfAttah;
	vec3 right = vec3::CrossProduct(InvertedNormal, up);
	Matrix44 transformForAttachPoint;
	auto data = transformForAttachPoint.data();
	data[0] = right.x;
	data[1] = right.y;
	data[2] = right.z;
	data[3] = 0.0;

	data[4] = up.x;
	data[5] = up.y;
	data[6] = up.z;
	data[7] = 0.0;

	data[8] = -InvertedNormal.x;
	data[9] = -InvertedNormal.y;
	data[10] = -InvertedNormal.z;
	data[11] = 0.0;

	data[12] = attachPosition.x;
	data[13] = attachPosition.y;
	data[14] = attachPosition.z;
	data[15] = 1.0;


	Matrix44 attachmentTrans;
	data = attachmentTrans.data();
	auto rightForAttach = vec3::CrossProduct(selfAttah->m_normal, selfAttah->m_up);
	vec3 normalForAttach = selfAttah->m_normal;
	data[0] = rightForAttach.x;
	data[1] = rightForAttach.y;
	data[2] = rightForAttach.z;
	data[3] = 0.0;

	data[4] = selfAttah->m_up.x;
	data[5] = selfAttah->m_up.y;
	data[6] = selfAttah->m_up.z;
	data[7] = 0.0;

	//use invert
	data[8] = -normalForAttach.x;
	data[9] = -normalForAttach.y;
	data[10] = -normalForAttach.z;
	data[11] = 0.0;

	data[12] = selfAttah->m_pos.x;
	data[13] = selfAttah->m_pos.y;
	data[14] = selfAttah->m_pos.z;
	data[15] = 1.0;

	auto result = transformForAttachPoint * attachmentTrans.inverted();
	Quaternion q;
	q.fromRotationMatrix(&result);
	m_node->setPos(result.getTranslation());
	m_node->setRotateQ(q);
	m_node->reCache();
}

void ControlPart::initAttachments()
{
	//forward backward
	m_attachment[0] = new Attachment(vec3(0.0, 0.0, blockSize / 2.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0) ,this);
	m_attachment[1] = new Attachment(vec3(0.0, 0.0, -blockSize / 2.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0) ,this);

	//right left
	m_attachment[2] = new Attachment(vec3(blockSize / 2.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0) ,this);
	m_attachment[3] = new Attachment(vec3(-blockSize / 2.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0) ,this);

	//up down
	m_attachment[4] = new Attachment(vec3(0.0, blockSize / 2.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0) ,this);
	m_attachment[5] = new Attachment(vec3(0.0, -blockSize / 2.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, 1.0) ,this);
}

Attachment * ControlPart::getAttachmentInfo(int index, vec3 & pos, vec3 & N, vec3 & up)
{
	auto mat = m_node->getLocalTransform();
	auto atta = m_attachment[index];
	vec4 a_pos = vec4(atta->m_pos, 1.0);
	vec4 a_n = vec4(atta->m_normal, 0.0);
	vec4 a_up = vec4(atta->m_up, 0.0);
	pos = mat.transofrmVec4(a_pos).toVec3();
	N = mat.transofrmVec4(a_n).toVec3();
	up = mat.transofrmVec4(a_up).toVec3();
	return m_attachment[index];
}

Attachment* ControlPart::getFirstAttachment()
{
	return m_attachment[5];
}

void ControlPart::addForwardBearing(BearPart* bearing)
{
	m_forwardBearing.push_back(bearing);
}

void ControlPart::addSidewardBearing(BearPart* bearing)
{
	m_sidewardBearing.push_back(bearing);
}

bool ControlPart::onKeyPress(int keyCode)
{
	if(!m_isActivate) return false;
	bool isKeyActivate = false;
	switch(keyCode)
	{
	    case TZW_KEY_W: 
		{
			m_forward += 1;
			isKeyActivate = true;
	    }
		break;
	    case TZW_KEY_S: 
		{
			m_forward -= 1;
			isKeyActivate = true;
	    }
		break;
	    case TZW_KEY_A: 
		{
			m_side -= 1;
			isKeyActivate = true;
	    }
		break;
	    case TZW_KEY_D: 
		{
			m_side += 1;
			isKeyActivate = true;
	    }
		break;
	}
	handleBearings();
	return false;
}

bool ControlPart::onKeyRelease(int keyCode)
{
	if(!m_isActivate) return false;
	bool isKeyActivate = false;
	switch(keyCode)
	{
	    case TZW_KEY_W: 
		{
			isKeyActivate = true;
			m_forward -= 1;
	    }
		break;
	    case TZW_KEY_S: 
		{
			isKeyActivate = true;
			m_forward += 1;
	    }
		break;
	    case TZW_KEY_A: 
		{
			m_side += 1;
			isKeyActivate = true;
	    }
		break;
	    case TZW_KEY_D: 
		{
			m_side -= 1;
			isKeyActivate = true;
	    }
		break;
	}
	if(isKeyActivate)
		handleBearings();
	return false;
}

bool ControlPart::getIsActivate() const
{
	return m_isActivate;
}

void ControlPart::setActivate(bool value)
{
	
	if (value) 
	{
		//GameWorld::shared()->getPlayer()->attachCamToGamePart(this);
		getNode()->setIsVisible(false);
	}
	else
	{	
		if(m_isActivate) 
		{
			//GameWorld::shared()->getPlayer()->attachCamToWorld();
		}
		getNode()->setIsVisible(true);
	}
	m_isActivate = value;
}

void ControlPart::onFrameUpdate(float delta)
{
}

void ControlPart::handleBearings()
{
	if (m_forward != 0) 
	{
		for (auto bearing : m_forwardBearing) 
		{
			bearing->m_constrain->enableAngularMotor(true, 10.0f *m_forward, 50);
		}
	} else 
	{
		for (auto bearing : m_forwardBearing) 
		{
			bearing->m_constrain->enableAngularMotor(false, 10.0f, 50);
		}
	}

	if (m_side != 0) 
	{
		for (auto bearing : m_sidewardBearing) 
		{
			bearing->m_constrain->enableAngularMotor(true, 1.0f *m_side, 50);
		}
	} else 
	{
		for (auto bearing : m_sidewardBearing) 
		{
			bearing->m_constrain->enableAngularMotor(true, 0.0f, 10000.0f);
		}
	}
}

int ControlPart::getType()
{
	return GAME_PART_CONTROL;
}

}
