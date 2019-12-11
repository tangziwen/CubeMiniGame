#include "PartSurfaceMgr.h"
#include "3D/Primitive/CubePrimitive.h"
#include <algorithm>
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsCompoundShape.h"
#include "Collision/PhysicsMgr.h"
#include "CylinderPart.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include <utility>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include "Utility/log/Log.h"
#include "Utility/file/Tfile.h"

namespace tzw
{
const float bearingGap = 0.00;
TZW_SINGLETON_IMPL(PartSurfaceMgr);
PartSurfaceMgr::PartSurfaceMgr()
{
	//load from Json
}


void PartSurfaceMgr::loadFromFile(std::string filePath)
{
	//Metal
	addItem("Metal Grid3", new PartSurface("Texture/metalgrid3-ue/metalgrid3_basecolor.png", 
		"Texture/metalgrid3-ue/metalgrid3_roughness.png",
		"Texture/metalgrid3-ue/metalgrid3_metallic.png", 
		"Texture/metalgrid3-ue/metalgrid3_normal-dx.png"));
	//scifi
	addItem("SCIFI", new PartSurface("Texture/military-panel1-ue/military-panel1-albedo.png", 
		"Texture/military-panel1-ue/military-panel1-rough.png",
		"Texture/military-panel1-ue/military-panel1-metalness.png", 
		"Texture/military-panel1-ue/military-panel1-nmap-dx.png"));


	//foam grip
	addItem("foam grip", new PartSurface("Texture/foam-grip1-ue/foam-grip1-albedo.png", 
		"Texture/foam-grip1-ue/foam-grip1-roughness.png",
		"Texture/foam-grip1-ue/foam-grip1-metallic.png", 
		"Texture/foam-grip1-ue/foam-grip1-normal-dx1.png"));

	//modern-tile1-ue
	addItem("Modern tile1", new PartSurface("Texture/modern-tile1-ue/modern-tile1-albedo.png", 
		"Texture/modern-tile1-ue/modern-tile1-roughness.png",
		"Texture/modern-tile1-ue/modern-tile1-metallic.png", 
		"Texture/modern-tile1-ue/modern-tile1-normal-dx.png"));

	//modern-tile1-ue
	addItem("Bamboo", new PartSurface(
		"Texture/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-albedo.png", 
		"Texture/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-roughness.png",
		"Texture/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-metal.png", 
		"Texture/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-normal.png"));

	//RustedIron
	addItem("Rusted Iron", new PartSurface(
		"Texture/RustedIron/rustediron2_basecolor.png", 
		"Texture/RustedIron/rustediron2_roughness.png",
		"Texture/RustedIron/rustediron2_metallic.png", 
		"Texture/RustedIron/rustediron2_normal.png"));

	//Flat
	addItem("Flat", new PartSurface(
		"Texture/BuiltInTexture/defaultBaseColor.png", 
		"Texture/BuiltInTexture/defaultRoughnessMap.png",
		"Texture/BuiltInTexture/defaultMetallic.png", 
		"Texture/BuiltInTexture/defaultNormalMap.png"));
}

PartSurface * PartSurfaceMgr::getItem(std::string name)
{
	return m_itemMap[name];
}

int PartSurfaceMgr::getItemAmount()
{
	return m_itemList.size();
}

PartSurface* PartSurfaceMgr::getItemByIndex(int index)
{
	return m_itemList[index];
}

void PartSurfaceMgr::addItem(std::string name, PartSurface* item)
{
	item->setName(name);
	m_itemList.push_back(item);
	m_itemMap[name] = item;
}
}
