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
	AttachmentInfo::AttachmentInfo(vec3 p, vec3 n, vec3 u, std::string l):
	pos(p),normal(n),up(u),locale(l)
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

	GameItem::GameItem()
{

}

	int GameItem::getTypeInInt()
	{
		return (int) m_type;
	}

	int GameItem::getThumbNailTextureId()
	{
		if(m_thumbNail)
		{
			return m_thumbNail->getTexture()->handle();
		}
		else
		{
			return 0;	
		}
	}
}
