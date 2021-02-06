#include "3D/Primitive/CubePrimitive.h"
#include <algorithm>
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsCompoundShape.h"
#include "Collision/PhysicsMgr.h"
#include "CylinderPart.h"
#include "3D/Primitive/CylinderPrimitive.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "GameItem.h"

namespace tzw
{
	AttachmentInfo::AttachmentInfo(vec3 p, vec3 n, vec3 u, std::string l, float collisionBoxSize):
	pos(p),normal(n),up(u),locale(l),collisionSize(collisionBoxSize)
	{
	}

	PhysicsInfo::PhysicsInfo():type(PhysicsInfoType::BoxShape),size(vec3(0,0,0)),mass(0)
	{
	}

	bool GameItem::isSpecialFunctionItem()
	{
		return (m_type == GamePartType::GAME_PART_LIFT) || (m_type == GamePartType::SPECIAL_PART_PAINTER) ||(m_type == GamePartType::SPECIAL_PART_DIGGER)  ;
	}

	bool GameItem::hasAttributePanel()
	{
		bool hasAttr = false;
		switch(m_type)
		{
		case GamePartType::GAME_PART_LIFT: hasAttr = true;break;
		case GamePartType::GAME_PART_CONTROL: break;
		case GamePartType::GAME_PART_THRUSTER: hasAttr = true;break;
		case GamePartType::GAME_PART_CANNON: hasAttr = true;break;
		case GamePartType::GAME_PART_BEARING: hasAttr = true;break;
		case GamePartType::GAME_PART_SPRING: hasAttr = true;break;
		default:
			break;
		}
		return hasAttr;
	}

	GameItem::GameItem(): m_type(), m_visualInfo(), m_physicsInfo(),
	m_thumbNail(nullptr), m_texture(nullptr),m_tintColor(vec3(1, 1, 1)),
	m_surfaceName("scuffed-plastic-1")
	{
	}

	GameItem::GameItem(const GameItem& other)
	{
		(*this) = other;
	}

	int GameItem::getTypeInInt()
	{
		return (int) m_type;
	}

	DeviceTexture * GameItem::getThumbNailTextureId()
	{
		if(m_thumbNail)
		{
			return m_thumbNail->getTexture()->handle();
		}
		else if(m_texture)
		{
			return m_texture->handle();	
		}else
		{
			return nullptr;
		}
	}
}
