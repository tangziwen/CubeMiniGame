#include "BuildingSystem.h"
#include "3D/Primitive/CubePrimitive.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "Base/GuidMgr.h"
#include "Base/Log.h"
#include "Chunk.h"
#include "Collision/PhysicsCompoundShape.h"
#include "Collision/PhysicsMgr.h"
#include "ControlPart.h"
#include "CylinderPart.h"
#include "Scene/SceneMgr.h"
#include "Utility/file/Tfile.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include <algorithm>

namespace tzw
{
	const float bearingGap = 0.00;
	TZW_SINGLETON_IMPL(BuildingSystem);

	BuildingSystem::BuildingSystem(): m_controlPart(nullptr), m_liftPart(nullptr), m_baseIndex(0)
	{
	}

	void
	BuildingSystem::createNewToeHold(vec3 pos)
	{
		auto newIsland = new Island(pos);
		m_IslandList.push_back(newIsland);
		auto part = new BlockPart();
		newIsland->m_node->addChild(part->getNode());
		newIsland->insert(part);
	}

	void
	BuildingSystem::removePartByAttach(Attachment* attach)
	{
		if (attach)
		{
			auto part = attach->m_parent;
			if(part == m_liftPart)
			{

				dropFromLift();
				m_liftPart->getNode()->removeFromParent();
				delete m_liftPart;
				m_liftPart = nullptr;
				//m_liftPart->m_parent->remove(m_liftPart);
			}
			else
			{
				auto island = part->m_parent;
				if (attach->m_parent->isConstraint())
				{
					auto bearing = static_cast<GameConstraint*>(attach->m_parent);
					m_bearList.erase(m_bearList.find(bearing));
				}
				part->getNode()->removeFromParent();
				island->remove(part);
				delete part;
			}

		}
	}

	void
	BuildingSystem::placeGamePart(GamePart* part, vec3 pos)
	{
		auto newIsland = new Island(pos);
		m_IslandList.push_back(newIsland);
		newIsland->m_node->addChild(part->getNode());
		newIsland->insert(part);
	}

	void
	BuildingSystem::attachGamePartToConstraint(GamePart* part, Attachment* attach)
	{
		vec3 pos, n, up;
		attach->getAttachmentInfoWorld(pos, n, up);

		auto island = new Island(pos + n * 0.5);
		auto constraint = static_cast<GameConstraint*>(attach->m_parent);
		constraint->m_b->m_parent->m_parent->addNeighbor(island);
		island->addNeighbor(constraint->m_b->m_parent->m_parent);
		m_IslandList.push_back(island);
		island->m_node->addChild(part->getNode());
		island->insert(part);
		island->m_islandGroup = attach->m_parent->m_parent->m_islandGroup;
		part->attachToFromOtherIsland(attach);
	}

	void
	BuildingSystem::attachGamePartNormal(GamePart* part, Attachment* attach)
	{
		if (attach->m_parent->getType() == GAME_PART_LIFT)
		{
			auto liftPart = dynamic_cast<LiftPart*>(attach->m_parent);
			vec3 pos, n, up;
			attach->getAttachmentInfoWorld(pos, n, up);
			auto newIsland = new Island(pos);
			m_IslandList.push_back(newIsland);
			newIsland->m_node->addChild(part->getNode());
			newIsland->insert(part);
			part->attachToFromOtherIsland(attach);
			newIsland->genIslandGroup();
			liftPart->setEffectedIsland(newIsland->m_islandGroup);
		}
		else
		{
			if (!attach->m_parent->isConstraint())
			{
				part->attachTo(attach);
				auto island = attach->m_parent->m_parent;
				island->m_node->addChild(part->getNode());
				island->insert(part);
			}
			else
			{
				attachGamePartToConstraint(part, attach);
			}
		}
	}

	void
	BuildingSystem::terrainForm(vec3 pos,
								vec3 dir,
								float dist,
								float value,
								float range)
	{
		std::vector<Drawable3D*> list;
		AABB aabb;
		aabb.update(vec3(pos.x - 10, pos.y - 10, pos.z - 10));
		aabb.update(vec3(pos.x + 10, pos.y + 10, pos.z + 10));
		g_GetCurrScene()->getRange(&list, aabb);
		if (!list.empty())
		{
			Drawable3DGroup group(&list[0], list.size());
			Ray ray(pos, dir);
			vec3 hitPoint;
			auto chunk = static_cast<Chunk*>(group.hitByRay(ray, hitPoint));
			if (chunk)
			{
				chunk->deformSphere(hitPoint, value, range);
			}
		}
	}

	vec3
	BuildingSystem::hitTerrain(vec3 pos, vec3 dir, float dist)
	{
		std::vector<Drawable3D*> list;
		AABB aabb;
		aabb.update(vec3(pos.x - 10, pos.y - 10, pos.z - 10));
		aabb.update(vec3(pos.x + 10, pos.y + 10, pos.z + 10));
		g_GetCurrScene()->getRange(&list, aabb);
		if (!list.empty())
		{
			Drawable3DGroup group(&list[0], list.size());
			Ray ray(pos, dir);
			vec3 hitPoint;
			auto chunk = static_cast<Chunk*>(group.hitByRay(ray, hitPoint));
			if (chunk)
			{
				return hitPoint;
			}
		}
		return vec3(999, 999, 999);
	}

	void
	BuildingSystem::placeLiftPart(vec3 wherePos)
	{
		auto part = new LiftPart();
		// TODO

		// auto newIsland = new Island(wherePos);
		// m_IslandList.insert(newIsland);
		// newIsland->m_node->addChild(part->getNode());
		// newIsland->insert(part);
		part->getNode()->setPos(wherePos);
		g_GetCurrScene()->addNode(part->getNode());
		m_liftPart = part;
	}

	void
	BuildingSystem::setCurrentControlPart(GamePart* controlPart)
	{
		auto part = dynamic_cast<ControlPart*>(controlPart);
		if (!m_controlPart)
		{
			if (part)
			{
				part->setActivate(true);
				m_controlPart = part;
			}
		}
		else
		{
			m_controlPart->setActivate(false);
			m_controlPart = nullptr;
		}
	}

	ControlPart*
	BuildingSystem::getCurrentControlPart() const
	{
		return m_controlPart;
	}

	GamePart*
	BuildingSystem::createPart(int type)
	{
		GamePart* resultPart = nullptr;
		switch (type)
		{
		case 0:
			resultPart = new BlockPart();
			break;
		case 1:
			resultPart = new CylinderPart();
			break;
		case 2:
			resultPart = new LiftPart();
			break;
		case 3:
			{
				auto control_part = new ControlPart();
				resultPart = control_part;
			}
			break;
		default: ;
		}
		return resultPart;
	}

	BearPart* BuildingSystem::placeBearingToAttach(Attachment* attachment)
	{
		vec3 pos, n, up;
		attachment->getAttachmentInfoWorld(pos, n, up);

		auto island = new Island(pos + n * 0.5);
		auto constraint = static_cast<GameConstraint*>(attachment->m_parent);
		m_IslandList.push_back(island);
		island->m_isSpecial = true;
		auto bear = new BearPart();
		bear->m_b = attachment;

		m_bearList.insert(bear);
		island->insert(bear);
		// create a indicate model
		auto cylinderIndicator = new CylinderPrimitive(0.15, 0.15, 0.1);
		cylinderIndicator->setColor(vec4(1.0, 1.0, 0.0, 0.0));
		auto mat = attachment->getAttachmentInfoMat44();
		Quaternion q;
		q.fromRotationMatrix(&mat);
		cylinderIndicator->setPos(mat.getTranslation());
		cylinderIndicator->setRotateQ(q);
		cylinderIndicator->reCache();
		bear->setNode(cylinderIndicator);
		bear->updateFlipped();
		bear->attachToFromOtherIslandAlterSelfIsland(attachment,
													bear->getFirstAttachment());
		attachment->m_connected = bear->getFirstAttachment();
		island->m_node->addChild(cylinderIndicator);
		island->m_islandGroup = attachment->m_parent->m_parent->m_islandGroup;
		return bear;
	}

	SpringPart*
	BuildingSystem::placeSpringToAttach(Attachment* attachment)
	{
		vec3 pos, n, up;
		attachment->getAttachmentInfoWorld(pos, n, up);

		auto island = new Island(pos + n * 0.5);
		auto constraint = static_cast<GameConstraint*>(attachment->m_parent);
		m_IslandList.push_back(island);
		island->m_isSpecial = true;
		auto bear = new SpringPart();
		bear->m_b = attachment;
		m_bearList.insert(bear);
		island->insert(bear);
		// create a indicate model
		auto cylinderIndicator = new CylinderPrimitive(0.15, 0.15, 0.5);
		cylinderIndicator->setColor(vec4(1.0, 1.0, 0.0, 0.0));
		auto mat = attachment->getAttachmentInfoMat44();
		Quaternion q;
		q.fromRotationMatrix(&mat);
		cylinderIndicator->setPos(mat.getTranslation());
		cylinderIndicator->setRotateQ(q);
		cylinderIndicator->reCache();
		bear->setNode(cylinderIndicator);
		bear->attachToFromOtherIslandAlterSelfIsland(attachment,
													bear->getFirstAttachment());
		attachment->m_connected = bear->getFirstAttachment();
		island->m_node->addChild(cylinderIndicator);
		island->m_islandGroup = attachment->m_parent->m_parent->m_islandGroup;
		return bear;
	}

	Island*
	BuildingSystem::createIsland(vec3 pos)
	{
		auto island = new Island(pos);
		m_IslandList.push_back(island);
		return island;
	}

	Attachment*
	BuildingSystem::rayTest(vec3 pos, vec3 dir, float dist)
	{
		std::vector<GamePart*> tmp;
		for (auto constraint : m_bearList)
		{
			tmp.push_back(constraint);
		}
		// search island
		for (auto island : m_IslandList)
		{
			for (auto iter : island->m_partList)
			{
				tmp.push_back(iter);
			}
		}
		// add extra lift part
		tmp.push_back(m_liftPart);
		std::sort(tmp.begin(), tmp.end(), [&](GamePart* left, GamePart* right)
		{
			float distl = left->getWorldPos().distance(pos);
			float distr = right->getWorldPos().distance(pos);
			return distl < distr;
		});
		for (auto iter : tmp)
		{
			auto island = iter->m_parent;
			auto node = iter->getNode();
			auto invertedMat = node->getTransform().inverted();
			vec4 dirInLocal = invertedMat * vec4(dir, 0.0);
			vec4 originInLocal = invertedMat * vec4(pos, 1.0);

			auto r = Ray(originInLocal.toVec3(), dirInLocal.toVec3());
			RayAABBSide side;
			vec3 hitPoint;
			auto isHit = r.intersectAABB(node->localAABB(), &side, hitPoint);
			GamePart* newPart = nullptr;
			if (isHit)
			{
				vec3 attachPos, attachNormal, attachUp;
				auto attach =
					iter->findProperAttachPoint(r, attachPos, attachNormal, attachUp);
				if (attach)
				{
					return attach;
				}
			}
		}
		// any island intersect can't find, return null
		return nullptr;
	}

	Island*
	BuildingSystem::rayTestIsland(vec3 pos, vec3 dir, float dist)
	{
		auto attach = rayTest(pos, dir, dist);
		if (attach)
			return attach->m_parent->m_parent;
		else
			return nullptr;
	}

	LiftPart*
	BuildingSystem::getLift() const
	{
		return m_liftPart;
	}

	ControlPart*
	BuildingSystem::getControlPart()
	{
		return m_controlPart;
	}

	void
	BuildingSystem::getIslandsByGroup(std::string islandGroup,
									std::vector<Island*>& groupList)
	{
		for (auto island : m_IslandList)
		{
			if (island->m_islandGroup == islandGroup)
				groupList.push_back(island);
		}
	}

void BuildingSystem::dump(std::string filePath)
	{
		rapidjson::Document doc;
		doc.SetObject();
		rapidjson::Value islandList(rapidjson::kArrayType);
		for (auto island : m_IslandList)
		{
			if (island->m_isSpecial)
				continue;
			rapidjson::Value islandObject(rapidjson::kObjectType);
			island->dump(islandObject, doc.GetAllocator());
			islandList.PushBack(islandObject, doc.GetAllocator());
		}
		doc.AddMember("islandList", islandList, doc.GetAllocator());

		rapidjson::Value constraintList(rapidjson::kArrayType);
		for (auto constraint : m_bearList)
		{
			rapidjson::Value bearingObj(rapidjson::kObjectType);
			bearingObj.AddMember("IslandGroup", constraint->m_parent->m_islandGroup, doc.GetAllocator());
			constraint->dump(bearingObj, doc.GetAllocator());
			constraintList.PushBack(bearingObj, doc.GetAllocator());
		}
		doc.AddMember("constraintList", constraintList, doc.GetAllocator());
		rapidjson::StringBuffer buffer;
		auto file = fopen(filePath.c_str(), "w");
		char writeBuffer[65536];
		rapidjson::FileWriteStream stream(file, writeBuffer, sizeof(writeBuffer));
		rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(stream);
		writer.SetIndent('\t', 1);
		doc.Accept(writer);
		fclose(file);
	}

	void
	BuildingSystem::load(std::string filePath)
	{
		rapidjson::Document doc;
		auto data = Tfile::shared()->getData(filePath, true);
		doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
		if (doc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d\n",
				filePath.c_str(),
				doc.GetParseError(),
				doc.GetErrorOffset());
			exit(0);
		}
		// island
		if (doc.HasMember("islandList"))
		{
			auto& items = doc["islandList"];
			for (unsigned int i = 0; i < items.Size(); i++)
			{
				auto& item = items[i];
				auto newIsland = new Island(vec3());
				m_IslandList.push_back(newIsland);
				newIsland->m_islandGroup = item["IslandGroup"].GetString();
				newIsland->load(item);
			}
		}

		// constraint
		if (doc.HasMember("constraintList"))
		{
			auto& items = doc["constraintList"];
			for (unsigned int i = 0; i < items.Size(); i++)
			{
				auto& item = items[i];
				std::string constraintType = item["Type"].GetString();
				auto GUID = item["from"].GetString();
				auto fromAttach =
					reinterpret_cast<Attachment*>(GUIDMgr::shared()->get(GUID));
				GameConstraint* constraint = nullptr;
				if (constraintType.compare("Spring") == 0)
				{
					constraint = placeSpringToAttach(fromAttach);
				}
				else if (constraintType.compare("Bearing") == 0)
				{
					constraint = placeBearingToAttach(fromAttach);
				}
				constraint->m_parent->m_islandGroup = item["IslandGroup"].GetString();
				// update constraint's attachment GUID
				auto& attachList = item["attachList"];
				for (unsigned int j = 0; j < attachList.Size(); j++)
				{
					auto& constraintAttach = attachList[j];
					constraint->getAttachment(j)->setGUID(
						constraintAttach["UID"].GetString());
				}
				if (item.HasMember("to"))
				{
					auto GUID = item["to"].GetString();
					auto toAttach =
						reinterpret_cast<Attachment*>(GUIDMgr::shared()->get(GUID));

					auto constraintAttach_target = reinterpret_cast<Attachment*>(
						GUIDMgr::shared()->get(toAttach->m_connectedGUID));
					toAttach->m_parent->attachToFromOtherIslandAlterSelfIsland(
						constraintAttach_target, toAttach);
					constraint->m_a = toAttach;
					if (toAttach->m_parent->getType() == GAME_PART_CYLINDER)
						printf("cylinder%s\n", toAttach->m_parent->getWorldPos().getStr().c_str());
				}
			}
		}
		auto island = m_IslandList[0];
		replaceToLift(island);
	}

	void
	BuildingSystem::updateBearing(float dt)
	{
		for (auto constrain : m_bearList)
		{
			constrain->updateTransform(dt);
		}
	}

	void
	BuildingSystem::flipBearingByHit(vec3 pos, vec3 dir, float dist)
	{
		std::vector<GamePart*> tmp;
		// search island
		for (auto island : m_IslandList)
		{
			for (auto iter : island->m_partList)
			{
				tmp.push_back(iter);
			}
		}
		std::sort(tmp.begin(), tmp.end(), [&](GamePart* left, GamePart* right)
		{
			float distl = left->getNode()->getWorldPos().distance(pos);
			float distr = right->getNode()->getWorldPos().distance(pos);
			return distl < distr;
		});
		for (auto iter : tmp)
		{
			auto island = iter->m_parent;
			auto node = iter->getNode();
			auto invertedMat = node->getTransform().inverted();
			vec4 dirInLocal = invertedMat * vec4(dir, 0.0);
			vec4 originInLocal = invertedMat * vec4(pos, 1.0);

			auto r = Ray(originInLocal.toVec3(), dirInLocal.toVec3());
			RayAABBSide side;
			vec3 hitPoint;
			auto isHit = r.intersectAABB(node->localAABB(), &side, hitPoint);
			GamePart* newPart = nullptr;
			if (isHit)
			{
				vec3 attachPos, attachNormal, attachUp;
				auto attach =
					iter->findProperAttachPoint(r, attachPos, attachNormal, attachUp);
				if (attach)
				{
					if (attach->m_parent->isConstraint())
					{
						auto bearPart = static_cast<BearPart*>(attach->m_parent);
						bearPart->m_isFlipped = !bearPart->m_isFlipped;
						bearPart->updateFlipped();
					}
				}
				return;
			}
		}
	}

	void
	BuildingSystem::placeItem(GameItem* item, vec3 pos, vec3 dir, float dist)
	{
	}

void BuildingSystem::dropFromLift()
	{
		// each island, for normal island we create a rigid, for constraint island, we create a constraint
		for (auto island : m_IslandList)
		{
			island->enablePhysics(true);
		}
		for(auto constraint :m_bearList)
		{
			constraint->enablePhysics(true);
		}
		if (m_liftPart)
		{
			// m_liftPart->getNode()->removeFromParent();
			// m_liftPart->m_parent->remove(m_liftPart);
		}
	}

void BuildingSystem::replaceToLiftByRay(vec3 pos, vec3 dir, float dist)
	{
		// put them back to the lift
		auto island = rayTestIsland(pos, dir, dist);
		if(island) 
		{
			// disable physics and put them back to lift position
			for (auto island : m_IslandList)
			{
				island->enablePhysics(false);
			}
			for(auto constraint :m_bearList)
			{
				constraint->enablePhysics(false);
			}
			replaceToLift(island);
		}
	}

void BuildingSystem::replaceToLift(Island* island)
{
	if (island)
	{
		vec3 attachPos, n, up;
		auto attach = m_liftPart->getFirstAttachment();
		attach->getAttachmentInfoWorld(attachPos, n, up);

		island->m_partList[0]->attachToFromOtherIslandAlterSelfIsland(
			attach, island->m_partList[0]->getBottomAttachment());

		m_liftPart->setEffectedIsland(island->m_islandGroup);
		std::set<Island *> closeSet;
		closeSet.insert(island);
		for (auto part : island->m_partList)
		{
			int count = part->getAttachmentCount();
			for (int i = 0; i < count; i++)
			{
				auto attach = part->getAttachment(i);
				auto connectedAttach = attach->m_connected;
				if (connectedAttach && connectedAttach->m_parent->isConstraint())
				{
					Attachment* other = nullptr;
					replaceToLift_R(connectedAttach->m_parent->m_parent, closeSet);
				}
			}
		}
	}
}

	void BuildingSystem::replaceToLift_R(Island* island, std::set<Island*>& closeList)
	{
		closeList.insert(island);
		//constraint island
		if(island->m_isSpecial)
		{
			auto constraint = static_cast<GameConstraint*>(island->m_partList[0]);
			int count = constraint->getAttachmentCount();
			for (int i = 0; i < count; i++)
			{
				auto attach = constraint->getAttachment(i);
				auto connectedAttach = attach->m_connected;
				if (connectedAttach)
				{
					if(closeList.find(connectedAttach->m_parent->m_parent) != closeList.end())
					{
						attach->m_parent->adjustFromOtherIslandAlterSelfIsland(connectedAttach,attach);
					} else
					{
						replaceToLift_R(connectedAttach->m_parent->m_parent, closeList);
					}
					
				}
			}
		}
		else
		{
			for (auto part : island->m_partList)
			{
				int count = part->getAttachmentCount();
				for (int i = 0; i < count; i++)
				{
					auto attach = part->getAttachment(i);
					auto connectedAttach = attach->m_connected;
					if (connectedAttach)
					{
						if(closeList.find(connectedAttach->m_parent->m_parent) != closeList.end())
						{
							attach->m_parent->adjustFromOtherIslandAlterSelfIsland(connectedAttach,attach);
						} else
						{
							replaceToLift_R(connectedAttach->m_parent->m_parent, closeList);
						}
					}
				}
			}
		}
	}

	// toDo
void BuildingSystem::findPiovtAndAxis(Attachment* attach,
									vec3 hingeDir,
									vec3& pivot,
									vec3& asix)
{
	auto part = static_cast<BlockPart*>(attach->m_parent);
	auto island = part->m_parent;
	auto islandInvertedMatrix = island->m_node->getLocalTransform().inverted();

	auto transform = part->getNode()->getLocalTransform();
	auto normalInIsland =
		transform.transofrmVec4(vec4(attach->m_normal, 0.0)).toVec3();

	pivot =
		transform.transofrmVec4(vec4(attach->m_pos + attach->m_normal * 0.05, 1.0))
				.toVec3();
	asix = islandInvertedMatrix.transofrmVec4(vec4(hingeDir, 0.0)).toVec3();
}

void BuildingSystem::tmpMoveWheel(bool isOpen)
{
	for (auto constrain : m_constrainList)
	{
		printf("move Wheel\n");
		constrain->enableAngularMotor(isOpen, 10, 100);
	}
}
}
