#include "ControlPart.h"
#include "3D/Primitive/CubePrimitive.h"
#include "AudioSystem/AudioSystem.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"
#include "Island.h"
#include "GameUISystem.h"
#include "NodeEditorNodes/ControlPartNode.h"
#include "Event/EventMgr.h"
#include "GameWorld.h"
#include "Base/GuidMgr.h"

namespace tzw
{
const float blockSize = 0.5;
ControlPart::ControlPart()
{
	

}
static Audio * audio;
static AudioEvent * event = NULL;
ControlPart::ControlPart(std::string itemName)
{
	m_isActivate = false;

	GamePart::initFromItemName(itemName);
	m_parent = nullptr;
	EventMgr::shared()->addFixedPiorityListener(this);
	ControlPart::generateName();
	audio = AudioSystem::shared()->createSound("Audio/engine_loop.wav");
	audio->setIsLooping(true);
}


void ControlPart::generateName()
{
	char formatName[512];
	sprintf_s(formatName, 512, TRC(u8"×ùÎ» %s"),genShortName().c_str());
	setName(formatName);
}

void ControlPart::AddOnVehicle(Vehicle* vehicle)
{
	m_graphNode = new ControlPartNode(this);
	vehicle->getEditor()->addNode(m_graphNode);
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

void ControlPart::initAttachments()
{
	//forward backward
	addAttachment(new Attachment(vec3(0.0, 0.0, blockSize / 2.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0) ,this));
	addAttachment(new Attachment(vec3(0.0, 0.0, -blockSize / 2.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0) ,this));

	//right left
	addAttachment(new Attachment(vec3(blockSize / 2.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0) ,this));
	addAttachment(new Attachment(vec3(-blockSize / 2.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0) ,this));

	//up down
	addAttachment(new Attachment(vec3(0.0, blockSize / 2.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0) ,this));
	addAttachment(new Attachment(vec3(0.0, -blockSize / 2.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, 1.0) ,this));
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


bool ControlPart::getIsActivate() const
{
	return m_isActivate;
}

void ControlPart::setActivate(bool value)
{
	m_isActivate = value;
	if (value) 
	{
		GameWorld::shared()->getPlayer()->sitDownToGamePart(this);
		audio->playWithOutCare();
	}
	else
	{	
		GameWorld::shared()->getPlayer()->standUpFromGamePart(this);
	}
}

void ControlPart::onFrameUpdate(float delta)
{
	if(m_node && m_node->getParent())
	{
		if(!event)
		{
			event = audio->play3D(getWorldPos());
		}
	}
}

void ControlPart::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
{
	GamePart::dump(partDocObj, allocator);
	//rapidjson::Value forwardBearingList(rapidjson::kArrayType);
	//for (auto bearing : m_forwardBearing) 
	//{
	//	rapidjson::Value bearingDoc(rapidjson::kObjectType);
	//	bearingDoc.AddMember("UID", std::string(bearing->getGUID()), allocator);
	//	forwardBearingList.PushBack(bearingDoc, allocator);
	//}
	//partDocObj.AddMember("forwardBearingList", forwardBearingList, allocator);

	//rapidjson::Value sideBearingList(rapidjson::kArrayType);
	//for (auto bearing : m_sidewardBearing) 
	//{
	//	rapidjson::Value bearingDoc(rapidjson::kObjectType);
	//	bearingDoc.AddMember("UID", std::string(bearing->getGUID()), allocator);
	//	sideBearingList.PushBack(bearingDoc, allocator);
	//}
	//partDocObj.AddMember("sideBearingList", sideBearingList, allocator);
}

void ControlPart::load(rapidjson::Value& partData)
{
	GamePart::load(partData);
	static_cast<ResNode *>(m_graphNode)->syncName();
	//auto& forwardBearingList = partData["forwardBearingList"];
	//for(unsigned int i = 0; i < forwardBearingList.Size(); i++) 
	//{
	//	auto &bearingData = forwardBearingList[i];
	//	auto bearPart =
	//		reinterpret_cast<BearPart*>(GUIDMgr::shared()->get(bearingData["UID"].GetString()));
	//	m_forwardBearing.push_back(bearPart);
	//}
}

Attachment* ControlPart::getAttachment(int index)
{
	return m_attachment[index];
}

ControlPart::~ControlPart()
{
	auto nodeEditor = m_vehicle->getEditor();
	nodeEditor->removeNode(m_graphNode);
	delete m_graphNode;
}

GraphNode* ControlPart::getGraphNode() const
{
	return m_graphNode;
}

GamePartType ControlPart::getType()
{
	return GamePartType::GAME_PART_CONTROL;
}

}
