#include "BuildingSystem.h"
#include "3D/Primitive/CubePrimitive.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "Base/GuidMgr.h"
#include "Utility/log/Log.h"
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

#include "AudioSystem/AudioSystem.h"
#include "GameUISystem.h"
#include "UIHelper.h"
#include "CannonPart.h"
#include "ThrusterPart.h"
#include "ItemMgr.h"
#include "ButtonPart.h"
#include "SwitchPart.h"

namespace tzw
{
	const float bearingGap = 0.00;

	BuildingSystem::BuildingSystem():m_controlPart(nullptr), m_liftPart(nullptr), m_baseIndex(0),m_isInXRayMode(false),m_storeIslandGroup(nullptr),m_staticVehicle(nullptr)
	{
		
	}

	void BuildingSystem::removePartByAttach(Attachment* attach)
	{
		if (attach)
		{
			auto part = attach->m_parent;
			removePart(part);
		}
	}

	void BuildingSystem::removePart(GamePart* part)
	{
		if (part)
		{
			if(part == m_liftPart)
			{
				removeLiftPart();
			}
			else
			{
				auto vehicle = part->getVehicle();
				auto island = part->m_parent;
				vehicle->removeGamePart(part);
				if(vehicle->getIslandList().empty())
				{
					if(vehicle == m_staticVehicle)
					{
						m_staticVehicle = nullptr;
					}
					if(m_vehicleList.find(vehicle)!= m_vehicleList.end())
					{
						m_vehicleList.erase(m_vehicleList.find(vehicle));
					}
					delete vehicle;
				}
			}
		}
    }
	void BuildingSystem::removeVehicle(Vehicle* vehicle)
	{
		if(vehicle == m_staticVehicle)
		{
			m_staticVehicle = nullptr;
		}
		if(m_vehicleList.find(vehicle)!= m_vehicleList.end())
		{
			m_vehicleList.erase(m_vehicleList.find(vehicle));
		}
		delete vehicle;
	}

    void BuildingSystem::placeGamePartStatic(GamePart* part, vec3 pos)
	{
		AudioSystem::shared()->playOneShotSound(AudioSystem::DefaultOneShotSound::CLINK);
		Island * newIsland = nullptr;
		float blockSize = 0.5f;
		vec3 resultWorldPos = vec3(int(std::round( pos.x / blockSize)) * blockSize, int(std::round(pos.y / blockSize)) * blockSize, 
			int(std::round(pos.z / blockSize)) * blockSize);
		if(!m_staticVehicle)
		{
			m_staticVehicle = new Vehicle();
			m_staticVehicle->genIslandGroup();
			newIsland = new Island(resultWorldPos, m_staticVehicle);
			newIsland->setIslandGroup(m_staticVehicle->getIslandGroup());
			newIsland->setIsStatic(true);
		}else
		{
			newIsland = m_staticVehicle->getIslandList()[0];
		}
		auto invMat = newIsland->m_node->getTransform().inverted();
		tlog("place static %s %s %s", pos.getStr().c_str(), resultWorldPos.getStr().c_str(), invMat.transformVec3(resultWorldPos).getStr().c_str());
		part->getNode()->setPos(invMat.transformVec3(resultWorldPos));
		newIsland->insert(part);
	}

	void
	BuildingSystem::attachGamePartToConstraint(GamePart* part, Attachment* attach, float degree)
	{
		
		vec3 pos, n, up;
		attach->getAttachmentInfoWorld(pos, n, up);

		auto island = new Island(pos + n * 0.5, attach->m_parent->m_parent->getVehicle());
		auto constraint = static_cast<GameConstraint*>(attach->m_parent);
		constraint->m_parent->addNeighbor(island);
		island->addNeighbor(constraint->m_parent);
		island->insert(part);
		island->setIslandGroup(attach->m_parent->getVehicle()->getIslandGroup());
		
		//part->attachToOtherIslandByAlterSelfPart(attach);
		part->attachToOtherIslandByAlterSelfIsland(attach, part->getFirstAttachment(), degree);
		//要看看约束是否在物理状态，它在，新Island也得在
		island->enablePhysics(constraint->isEnablePhysics());
		constraint->updateConstraintState();
	}

	void
	BuildingSystem::attachGamePart(GamePart* part, Attachment* attach, float degree, int index)
	{
		AudioSystem::shared()->playOneShotSound(AudioSystem::DefaultOneShotSound::CLINK);
		//first part.build a vehicle
		if (attach->m_parent->getType() == GamePartType::GAME_PART_LIFT)
		{
			Vehicle * vehicle = new Vehicle();
			auto liftPart = dynamic_cast<LiftPart*>(attach->m_parent);
			vec3 pos, n, up;
			attach->getAttachmentInfoWorld(pos, n, up);
			auto newIsland = new Island(pos, vehicle);
			vehicle->genIslandGroup();
			newIsland->insertAndAdjustAttach(part, attach, index);
			newIsland->setIslandGroup(vehicle->getIslandGroup());
			//vehicle->
			liftPart->setEffectedIsland(vehicle);
			m_vehicleList.emplace(vehicle);
		}
		else
		{
			if (!attach->m_parent->isConstraint())//内部连接
			{
				part->attachTo(attach, degree, index);
				auto island = attach->m_parent->m_parent;
				island->insert(part);
			}
			else
			{
				attachGamePartToConstraint(part, attach, degree);
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
				AudioSystem::shared()->playOneShotSound(AudioSystem::DefaultOneShotSound::DIGGING);
				chunk->deformSphere(hitPoint, value, range);
			}
		}
	}

	void BuildingSystem::terrainPaint(vec3 pos, vec3 dir, float dist, int matIndex, float range)
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
				chunk->paintSphere(hitPoint, matIndex, range);
			}
		}
	}

	vec3 BuildingSystem::hitTerrain(vec3 pos, vec3 dir, float dist)
	{
		std::vector<Drawable3D*> list;
		AABB aabb;
		aabb.update(vec3(pos.x - dist, pos.y - dist, pos.z - dist));
		aabb.update(vec3(pos.x + dist, pos.y + dist, pos.z + dist));
		g_GetCurrScene()->getRange(&list, aabb);
		if (!list.empty())
		{
			for(auto i = list.begin(); i != list.end();)
			{
				if(!dynamic_cast<Chunk*>(*i)) 
				{
					i = list.erase(i);
				}else
				{
					i++;
				}
			}
			if(!list.empty())
			{
				Drawable3DGroup group(&list[0], list.size());
				Ray ray(pos, dir);
				vec3 hitPoint;
				if (group.hitByRay(ray, hitPoint))
				{
					
					return hitPoint;
				}
			}
		}
		return vec3(-999999, -999999, -999999);
	}

	void
	BuildingSystem::placeLiftPart(vec3 wherePos)
	{
		if(m_liftPart) 
		{
			UIHelper::shared()->showFloatTips(TR(u8"升降台只能放一个"));
            return;
        }
		auto part = new LiftPart();
		part->setPos(wherePos);
		m_liftPart = part;
		//收纳状态
		if(m_storeIslandGroup)
		{
			auto& islandList = m_storeIslandGroup->getIslandList();
			//getIslandsByGroup(m_storeIslandGroup,islandList);
			// disable physics and put them back to lift position
			for (auto island : islandList)
			{
				island->m_node->setIsVisible(true);
				island->enablePhysics(false);
				//recover from building rotate
				island->recoverFromBuildingRotate();
			}
			for(auto constraint :m_storeIslandGroup->getConstraintList())
			{
				constraint->enablePhysics(false);
			}
		
			tempPlace(islandList[0], m_liftPart);
			////readjust
			auto attach = replaceToLiftIslands(m_storeIslandGroup);
			replaceToLift(attach->m_parent->m_parent, attach, m_liftPart);
			//清空收纳对象
			m_storeIslandGroup = nullptr;
		}
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

	int BuildingSystem::getGamePartTypeInt(GamePart* part)
	{
		return int(part->getType());
	}

	ControlPart*
	BuildingSystem::getCurrentControlPart() const
	{
		return m_controlPart;
	}

	GamePart*
	BuildingSystem::createPart(int type, std::string itemName)
	{
		ItemMgr::shared()->getItem(itemName);
		GamePart* resultPart = nullptr;
		switch ((GamePartType)type)
		{
        case GamePartType::GAME_PART_BLOCK:
			resultPart = new BlockPart(itemName);
			break;
		case GamePartType::GAME_PART_CYLINDER:
			resultPart = new CylinderPart();
			break;
		case GamePartType::GAME_PART_LIFT:
			resultPart = new LiftPart();
			break;
        case GamePartType::GAME_PART_CONTROL:
			{
				auto control_part = new ControlPart(itemName);
				resultPart = control_part;
			}
			break;
		case GamePartType::GAME_PART_CANNON:
			resultPart = new CannonPart(itemName);
			break;
        case GamePartType::GAME_PART_THRUSTER:
			resultPart = new ThrusterPart(itemName);
			break;
        case GamePartType::GAME_PART_BUTTON:
			resultPart = new ButtonPart(itemName);
			break;
        case GamePartType::GAME_PART_SWITCH:
			resultPart = new SwitchPart(itemName);
			break;
		default:
			assert(0);
			;
		}
		return resultPart;
	}

	BearPart* BuildingSystem::placeBearingToAttach(Attachment* attachment, std::string itemName)
	{
		vec3 pos, n, up;
		attachment->getAttachmentInfoWorld(pos, n, up);

		auto island = new Island(pos + n * 0.5, attachment->m_parent->m_parent->getVehicle());
		auto constraint = static_cast<GameConstraint*>(attachment->m_parent);
		island->m_isSpecial = true;
		auto bear = new BearPart(itemName);
		bear->m_b = attachment;
		bear->m_parent = island;
		island->insert(bear);
		bear->updateFlipped();
		bear->attachToOtherIslandByAlterSelfIsland(attachment,
													bear->getFirstAttachment(), 0);
		attachment->m_connected = bear->getFirstAttachment();
		auto euler = island->m_node->getRotateE();
		if(!attachment->m_connected->m_parent->m_parent) 
		{
			tlog("wrong");
		}
		attachment->m_parent->m_parent->addNeighbor(island);
		island->addNeighbor(attachment->m_parent->m_parent);
		island->setIslandGroup( attachment->m_parent->getVehicle()->getIslandGroup());
		
		return bear;
	}

	SpringPart*
	BuildingSystem::placeSpringToAttach(Attachment* attachment, std::string itemName)
	{
		vec3 pos, n, up;
		attachment->getAttachmentInfoWorld(pos, n, up);

		auto island = new Island(pos + n * 0.5, attachment->m_parent->m_parent->getVehicle());
		auto constraint = static_cast<GameConstraint*>(attachment->m_parent);
		island->m_isSpecial = true;
		auto spring = new SpringPart(itemName);
		spring->m_b = attachment;
		spring->m_parent = island;
		
		spring->attachToOtherIslandByAlterSelfIsland(attachment,
												spring->getFirstAttachment(), 0);
		island->insert(spring);
		attachment->m_connected = spring->getFirstAttachment();
		
		attachment->m_parent->m_parent->addNeighbor(island);
		island->addNeighbor(attachment->m_parent->m_parent);
		if(!attachment->m_connected->m_parent->m_parent) 
		{
			tlog("wrong");
		}
		
		island->setIslandGroup(attachment->m_parent->getVehicle()->getIslandGroup());
		return spring;
	}

	Attachment*
	BuildingSystem::rayTest(vec3 pos, vec3 dir, float dist)
	{
		std::vector<GamePart*> tmp;
		for(auto vehicle : m_vehicleList)
		{
			for (auto constraint : vehicle->getConstraintList())
			{
				tmp.push_back(constraint);
			}
			// search island
			for (auto island : vehicle->getIslandList())
			{
				if(island->m_isSpecial) continue;
				//被收纳的island不会响应
				if(m_storeIslandGroup && island->getIslandGroup() == m_storeIslandGroup->getIslandGroup()) continue;
				for (auto iter : island->m_partList)
				{
					tmp.push_back(iter);
				}
			}
		}


		if(m_staticVehicle)
		{
			// search static
			for (auto island : m_staticVehicle->getIslandList())
			{
				for (auto iter : island->m_partList)
				{
					tmp.push_back(iter);
				}
			}
		}


		if(m_liftPart)// add extra lift part
		{
			tmp.push_back(m_liftPart);
		}
		
		std::sort(tmp.begin(), tmp.end(), [&](GamePart* left, GamePart* right)
		{
			float distl = left->getWorldPos().distance(pos);
			float distr = right->getWorldPos().distance(pos);
			return distl < distr;
		});
		for (auto iter : tmp)
		{
			auto r = Ray(pos, dir);
			auto isHit = iter->isHit(r);
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

	GamePart* BuildingSystem::rayTestPart(vec3 pos, vec3 dir, float dist)
	{
		std::vector<GamePart*> tmp;
		for(auto vehicle : m_vehicleList)
		{
			for (auto constraint : vehicle->getConstraintList())
			{
				tmp.push_back(constraint);
			}
			// search island
			for (auto island : vehicle->getIslandList())
			{
				if(island->m_isSpecial) continue;
				//被收纳的island不响应
				if(m_storeIslandGroup && island->getIslandGroup() == m_storeIslandGroup->getIslandGroup()) continue;
				for (auto iter : island->m_partList)
				{
					tmp.push_back(iter);
				}
			}
		}

		if(m_staticVehicle)
		{
			// search static
			for (auto island : m_staticVehicle->getIslandList())
			{
				for (auto iter : island->m_partList)
				{
					tmp.push_back(iter);
				}
			}
		}
		if(m_liftPart)// add extra lift part
		{
			tmp.push_back(m_liftPart);
		}
		
		std::sort(tmp.begin(), tmp.end(), [&](GamePart* left, GamePart* right)
		{
			float distl = left->getWorldPos().distance(pos);
			float distr = right->getWorldPos().distance(pos);
			return distl < distr;
		});
		for (auto iter : tmp)
		{
			if (iter->isHit(Ray(pos, dir)))
			{
				return iter;
			}
		}
		// any island intersect can't find, return null
		return nullptr;
	}

	GamePart* BuildingSystem::rayTestPartAny(vec3 pos, vec3 dir, float dist)
	{
		if(!isIsInXRayMode())
		{
			return rayTestPart(pos, dir, dist);
		}
		else
		{
			return rayTestPartXRay(pos, dir, dist);
		}
	}

	GamePart* BuildingSystem::rayTestPartXRay(vec3 pos, vec3 dir, float dist)
	{
		std::vector<GamePart*> tmp;
		for(auto vehicle : m_vehicleList)
		{
			for (auto constraint : vehicle->getConstraintList())
			{
				tmp.push_back(constraint);
			}
		}

		if(m_liftPart)// add extra lift part
		{
			tmp.push_back(m_liftPart);
		}
		std::sort(tmp.begin(), tmp.end(), [&](GamePart* left, GamePart* right)
		{
			float distl = left->getWorldPos().distance(pos);
			float distr = right->getWorldPos().distance(pos);
			return distl < distr;
		});
		for (auto iter : tmp)
		{
			if (iter->isHit(Ray(pos, dir)))
			{
				return iter;
			}
		}
		// any island intersect can't find, return null
		return nullptr;
	}

	Island*
	BuildingSystem::rayTestIsland(vec3 pos, vec3 dir, float dist)
	{
		auto part = rayTestPart(pos, dir, dist);
		if (part)
			return part->m_parent;
		else
			return nullptr;
	}

	LiftPart*
	BuildingSystem::getLift() const
	{
		return m_liftPart;
	}

	void BuildingSystem::removeLiftPart()
	{
		dropFromLift();
		m_liftPart->getFirstAttachment()->breakConnection();
		delete m_liftPart;
		m_liftPart = nullptr;
	}

	void BuildingSystem::addThruster(GamePart* thruster)
	{
		m_thrusterList.insert(thruster);
	}

	void BuildingSystem::removeThruster(GamePart* thrsuter)
	{
		m_thrusterList.erase(m_thrusterList.find(thrsuter));
	}

	void BuildingSystem::liftStore(GamePart* part)
	{
		if(part == m_liftPart) return;
		if(part->getVehicle() == m_staticVehicle) return;
		if(m_liftPart)//already place lift part
		{
			if(part != m_liftPart)
			{
				//teleport it
				auto v = part->getVehicle();
				v->disablePhysics();
				auto firstIsland = v->getIslandList()[0];
				tempPlace(firstIsland, m_liftPart);
				//readjust
				auto attach = replaceToLiftIslands(firstIsland->getVehicle());
				replaceToLift(attach->m_parent->m_parent, attach, m_liftPart);
			}
			return;
		}
		//收纳起来
		m_storeIslandGroup = part->getVehicle();
		auto& group = m_storeIslandGroup->getIslandList();
		//禁用物理
		for (auto island : group)
		{
			island->enablePhysics(false);
			//recover from building rotate
			island->recoverFromBuildingRotate();
		}
		for(auto constraint :m_storeIslandGroup->getConstraintList())
		{
			constraint->enablePhysics(false);
		}
	}

	void
	BuildingSystem::getIslandsByGroup(std::string islandGroup,
									std::vector<Island*>& groupList)
	{
		for(auto vehicle : m_vehicleList)
		{
			for (auto island : vehicle->getIslandList())
			{
				if (island->getIslandGroup() == islandGroup)
					groupList.push_back(island);
			}
		}

	}

	void BuildingSystem::dumpVehicle(std::string filePath)
	{
		
		if(!m_liftPart->getFirstAttachment()->m_connected)
		{
			tlog("you must dock the vehilce on Lift to dump");
			return;
		}
		auto vehicle = m_liftPart->getFirstAttachment()->m_connected->m_parent->getVehicle();
		rapidjson::Document doc;
		doc.SetObject();
		rapidjson::Value islandList(rapidjson::kArrayType);
		auto& currIslandList = vehicle->getIslandList();
		for (auto island : currIslandList)
		{
			if (island->m_isSpecial)
				continue;
			//we need record the building rotation!!!!!
			island->recordBuildingRotate();
			rapidjson::Value islandObject(rapidjson::kObjectType);
			island->dump(islandObject, doc.GetAllocator());
			islandList.PushBack(islandObject, doc.GetAllocator());
		}
		doc.AddMember("IslandGroup", vehicle->getIslandGroup(), doc.GetAllocator());
		doc.AddMember("islandList", islandList, doc.GetAllocator());

		//constraint
		rapidjson::Value constraintList(rapidjson::kArrayType);
		auto& vConstraint = vehicle->getConstraintList();
		for (auto constraint : vConstraint)
		{
			rapidjson::Value bearingObj(rapidjson::kObjectType);
			bearingObj.AddMember("IslandGroup", constraint->m_parent->getIslandGroup(), doc.GetAllocator());
			constraint->dump(bearingObj, doc.GetAllocator());
			constraintList.PushBack(bearingObj, doc.GetAllocator());
		}
		doc.AddMember("constraintList", constraintList, doc.GetAllocator());


		//Node Editor
		vehicle->getEditor()->dump(doc, doc.GetAllocator());
		
		
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
	BuildingSystem::loadVehicle(std::string filePath)
	{
		Vehicle * vehicle = new Vehicle();
		rapidjson::Document doc;
		auto data = Tfile::shared()->getData(filePath, true);
		doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
		if (doc.HasParseError())
		{
			tlog("[error] get json data err! %s %d offset %d",
				filePath.c_str(),
				doc.GetParseError(),
				doc.GetErrorOffset());
			exit(1);
		}
		removeLiftConnected();
		std::vector<Island * > tmp_islandList;
		vehicle->setIslandGroup(doc["IslandGroup"].GetString());
		m_vehicleList.insert(vehicle);
		// island
		if (doc.HasMember("islandList"))
		{
			auto& items = doc["islandList"];
			//可能当前场景已经有散落的原载具，它们不在升降机上，也要搞死
			removeByGroup(vehicle->getIslandGroup());
			for (unsigned int i = 0; i < items.Size(); i++)
			{
				auto& item = items[i];
				auto newIsland = new Island(vec3(), vehicle);
				newIsland->setIslandGroup(vehicle->getIslandGroup());
				newIsland->load(item);
				tmp_islandList.emplace_back(newIsland);
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
				if (constraintType == "Spring")
				{
					constraint = placeSpringToAttach(fromAttach, item["ItemName"].GetString());
				}
				else if (constraintType == "Bearing")
				{
					
					constraint = placeBearingToAttach(fromAttach, item["ItemName"].GetString());
					auto bear = reinterpret_cast<BearPart*>(constraint);
					if(item.HasMember("isSteering")) 
					{
						bear->setIsSteering(item["isSteering"].GetBool());
					}
					
					if(item.HasMember("isAngleLimit"))
					{
						
						bear->setAngleLimit(item["isAngleLimit"].GetBool(), 
							item["AngleLimitLow"].GetDouble(), item["AngleLimitHigh"].GetDouble());
					}
				}
				constraint->m_parent->setIslandGroup(vehicle->getIslandGroup());
				constraint->setGUID(item["UID"].GetString());
				constraint->setName(item["Name"].GetString());
				// update constraint's attachment GUID
				//auto& attachList = item["attachList"];
				//for (unsigned int j = 0; j < attachList.Size(); j++)
				//{
				//	auto& constraintAttach = attachList[j];
				//	constraint->getAttachment(j)->setGUID(
				//		constraintAttach["UID"].GetString());
				//}
				constraint->loadAttach(item);
				if (item.HasMember("to"))
				{
					auto GUID = item["to"].GetString();
					auto toAttach =
						reinterpret_cast<Attachment*>(GUIDMgr::shared()->get(GUID));

					auto constraintAttach_target = reinterpret_cast<Attachment*>(
						GUIDMgr::shared()->get(toAttach->m_connectedGUID));
					toAttach->m_parent->attachToOtherIslandByAlterSelfIsland(
						constraintAttach_target, toAttach, toAttach->m_degree);
					printf("aaaaa %p\n",constraint->m_a);
					constraint->m_a = toAttach;
				}
				constraint->load(item);
			}
		}

		//Node Editor
		vehicle->getEditor()->load(doc["NodeGraph"]);

		if(m_liftPart)
		{
			auto firstIsland = tmp_islandList[0];
			tempPlace(firstIsland, m_liftPart);
			//readjust
			auto attach = replaceToLiftIslands(firstIsland->getVehicle());
			replaceToLift(attach->m_parent->m_parent, attach, m_liftPart);
		}else 
		{
			// for no run test, if we not yet loaded the world & placed the lift, we still can debug the Node Editor problems.
			//for debugging purpose
			GameUISystem::shared()->setIsShowNodeEditor(true);
		}
	}

	void BuildingSystem::clearStatic()
	{
		if(m_staticVehicle)
		{
			SAFE_DELETE(m_staticVehicle);
		}
	}

	void BuildingSystem::loadStatic(rapidjson::Value &doc)
	{
		auto& items = doc["StaticIslandList"];
		if(items.Size() > 0)
		{
			std::string islandGroup = items[0]["IslandGroup"].GetString();
			removeByGroup(islandGroup);
			if(m_staticVehicle)
			{
				delete m_staticVehicle;
			}
			for (unsigned int i = 0; i < items.Size(); i++)
			{
				m_staticVehicle = new Vehicle();
				m_staticVehicle->genIslandGroup();
				auto& item = items[i];
				auto newIsland = new Island(vec3(), m_staticVehicle);
				//m_IslandList.push_back(newIsland);
				newIsland->setIslandGroup(m_staticVehicle->getIslandGroup());
				newIsland->load(item);
				newIsland->setIsStatic(true);
				newIsland->enablePhysics(true);
			}
		}

	}

	void BuildingSystem::dumpStatic(rapidjson::Value &doc, rapidjson::Document::AllocatorType& allocator)
	{
		if(!m_staticVehicle) return;
		rapidjson::Value islandList(rapidjson::kArrayType);
		for(auto island :m_staticVehicle->getIslandList())
		{
			rapidjson::Value islandObject(rapidjson::kObjectType);
			island->dump(islandObject, allocator);
			islandList.PushBack(islandObject, allocator);
		}
		doc.AddMember("StaticIslandList", islandList, allocator);
	}

	void BuildingSystem::updateBearing(float dt)
	{
		for (auto vehicle : m_vehicleList)
		{
			vehicle->update(dt);
		}
	}

	void BuildingSystem::removeLiftConnected()
	{
		if(!m_liftPart) return;
		std::vector<Island *> groupList;
		if(m_liftPart->m_effectedIslandGroup)
		{
			removeVehicle(m_liftPart->m_effectedIslandGroup);
			m_liftPart->m_effectedIslandGroup = nullptr;
		}
	}

	void BuildingSystem::removeAll()
	{
		for(auto vehicle : m_vehicleList)
		{
			delete vehicle;
		}
		m_vehicleList.clear();
	}

	void BuildingSystem::removeByGroup(std::string islandGroup)
	{
		for(auto i = m_vehicleList.begin(); i != m_vehicleList.end();)
		{
			auto vehicle = (*i);
			if(vehicle->getIslandGroup() == islandGroup)
			{
				i = m_vehicleList.erase(i);
				delete vehicle;
			}
			else
			{
				 i++;
			}
		}
	}

	void BuildingSystem::removeIsland(Island* island)
	{
		auto vehicle = island->getVehicle();
		vehicle->removeIsland(island);
	}

	void BuildingSystem::update(float dt)
	{
		updateBearing(dt);

		//update thrusters
		for(auto thruster : m_thrusterList)
		{
			static_cast<ThrusterPart * >(thruster)->updateForce(dt);
		}
	}

	std::set<Vehicle*>& BuildingSystem::getVehicleList()
	{
		return m_vehicleList;
	}

	bool BuildingSystem::isIsInXRayMode() const
	{
		return m_isInXRayMode;
	}

	void BuildingSystem::setIsInXRayMode(const bool isInXRayMode)
	{
		m_isInXRayMode = isInXRayMode;
	}

	Vehicle* BuildingSystem::getStoreIslandGroup() const
	{
		return m_storeIslandGroup;
	}

	void BuildingSystem::setStoreIslandGroup(Vehicle* storeIslandGroup)
	{
		m_storeIslandGroup = storeIslandGroup;
	}

	void
	BuildingSystem::flipBearingByHit(vec3 pos, vec3 dir, float dist)
	{
		GamePart * part = nullptr;
		if(!isIsInXRayMode())
		{
			part = rayTestPart(pos, dir, dist);
		} else 
		{
			rayTestPartXRay(pos, dir, dist);
		}
		if(part && part->getType() == GamePartType::GAME_PART_BEARING)
		{
			auto bearPart = static_cast<BearPart*>(part);
			bearPart->m_isFlipped = !bearPart->m_isFlipped;
			bearPart->updateFlipped();
		}
	}

	void BuildingSystem::flipBearing(GamePart* gamePart)
	{
		if(gamePart && gamePart->getType() == GamePartType::GAME_PART_BEARING)
		{
			auto bearPart = static_cast<BearPart*>(gamePart);
			bearPart->m_isFlipped = !bearPart->m_isFlipped;
			bearPart->updateFlipped();
		}
	}

	void
	BuildingSystem::placeItem(GameItem* item, vec3 pos, vec3 dir, float dist)
	{
	}

void BuildingSystem::dropFromLift()
	{
		Vehicle * currVehicle = nullptr;
		for(auto vehicle : m_vehicleList)
		{
			if(vehicle == m_liftPart->m_effectedIslandGroup)
			{
				currVehicle = vehicle;
			}
		}
		if(!currVehicle) return;
		currVehicle->enablePhysics();
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
			auto & islandList = island->getVehicle()->getIslandList();
			// disable physics and put them back to lift position
			for (auto island : islandList)
			{
				island->enablePhysics(false);
				//recover from building rotate
				island->recoverFromBuildingRotate();
			}
			auto & constraintList = island->getVehicle()->getConstraintList();
			for(auto constraint :constraintList)
			{
				constraint->enablePhysics(false);
			}
		
			tempPlace(island, m_liftPart);
			////readjust
			auto attach = replaceToLiftIslands(island->getVehicle());
			replaceToLift(attach->m_parent->m_parent, attach, m_liftPart);
		}
	}

void BuildingSystem::replaceToLift(Island* island, Attachment * attachment, LiftPart * targetLift)
{
	if (!island) return;
	vec3 attachPos, n, up;
	auto attach = targetLift->getFirstAttachment();
	attach->getAttachmentInfoWorld(attachPos, n, up);

	if(!attachment)
	{
	island->m_partList[0]->adjustToOtherIslandByAlterSelfIsland(
		attach, island->m_partList[0]->getBottomAttachment(), 0);
	}else 
	{
		attachment->m_parent->adjustToOtherIslandByAlterSelfIsland(attach, attachment, 0);
	}
	targetLift->setEffectedIsland(island->getVehicle());
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
				connectedAttach->m_parent->adjustToOtherIslandByAlterSelfIsland(attach, connectedAttach, connectedAttach->m_degree);
				replaceToLift_R(connectedAttach->m_parent->m_parent, closeSet, targetLift);
			}
		}
	}
}

	void BuildingSystem::tempPlace(Island* island,LiftPart * targetLift)
	{
		if (!island) return;
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
					replaceToLift_R(connectedAttach->m_parent->m_parent, closeSet, targetLift);
				}
			}
		}
	}

	Attachment * BuildingSystem::replaceToLiftIslands(Vehicle * vehicle)
	{
		if(!vehicle) return nullptr;
		auto& islandGroup = vehicle->getIslandList();

		std::vector<Attachment *> attachmentList;
		AABB aabb;
		for(auto island : islandGroup)
		{
			if(island->m_isSpecial) continue;// constraint island is not allowed

			for(auto part: island->m_partList)
			{
				for(int i = 0; i < part->getAttachmentCount(); i ++)
				{
					auto attach = part->getAttachment(i);
					if(!attach->m_connected) 
					{
						attachmentList.push_back(attach);
					}
				}
				aabb.update(part->getWorldPos());
			}
		}
		auto center = aabb.centre();
		std::sort(attachmentList.begin(), attachmentList.end(), 
			[center](Attachment * l, Attachment * r) -> bool
		{
			vec3 pl,nl,ul;
			l->getAttachmentInfoWorld(pl,nl,ul);
			vec3 pr,nr,ur;
			r->getAttachmentInfoWorld(pr, nr, ur);
			float distFactorL = vec3(pl.x, 0, pl.z).distance(vec3(center.x, 0, center.z));
			float distFactorR = vec3(pr.x, 0, pr.z).distance(vec3(center.x, 0, center.z));

			float lowFactorL = pl.y - center.y;
			float lowFactorR = pr.y - center.y;
			return (distFactorL + lowFactorL) < (distFactorR + lowFactorR);
		});
		return attachmentList[0];//这里有风险，可能bottom也被连接了
	}

	void BuildingSystem::replaceToLift_R(Island* island, std::set<Island*>& closeList, LiftPart * targetLift)
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
				if (connectedAttach && connectedAttach->m_parent != targetLift)
				{
					if(closeList.find(connectedAttach->m_parent->m_parent) != closeList.end())
					{
						//attach->m_parent->adjustToOtherIslandByAlterSelfIsland(connectedAttach,attach, 0);
					} else
					{
						connectedAttach->m_parent->adjustToOtherIslandByAlterSelfIsland(attach, connectedAttach, connectedAttach->m_degree);
						replaceToLift_R(connectedAttach->m_parent->m_parent, closeList, targetLift);
					}
					
				}
			}
		}
		else
		{
			for (auto part : island->m_partList)
			{
				int count = part->getAttachmentCount();
				Attachment * connectedAttach = nullptr;
				for (int i = 0; i < count; i++)
				{
					auto attach = part->getAttachment(i);
					connectedAttach = attach->m_connected;
					if (connectedAttach && connectedAttach->m_parent != targetLift)
					{
						if(closeList.find(connectedAttach->m_parent->m_parent) != closeList.end())
						{
							//attach->m_parent->adjustToOtherIslandByAlterSelfIsland(connectedAttach,attach, attach->m_degree);
						} else
						{
							connectedAttach->m_parent->adjustToOtherIslandByAlterSelfIsland(attach, connectedAttach, connectedAttach->m_degree);
							replaceToLift_R(connectedAttach->m_parent->m_parent, closeList, targetLift);
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
}
