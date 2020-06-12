#include "GamePart.h"
#include "Island.h"
#include "BearPart.h"
#include "2D/GUISystem.h"
#include "ItemMgr.h"
#include "Texture/TextureMgr.h"
#include "3D/Primitive/CubePrimitive.h"
#include "3D/Primitive/CylinderPrimitive.h"
#include "3D/Model/Model.h"
#include "PartSurface.h"
#include "PartSurfaceMgr.h"
#include "3D/Primitive/RightPrismPrimitive.h"
#include "Engine/DebugSystem.h"

namespace tzw
{

	Drawable3D * GamePart::getNode() const
	{
		return m_node;
	}

	void GamePart::setNode(Drawable3D * node)
	{
		m_node = node;
	}

	PhysicsShape * GamePart::getShape()
	{
		return m_shape;
	}

	Attachment * GamePart::findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & up)
	{
		float minDist = 99999999.0;
		int resultIndx = -1;
		int count = 0;
		vec3 theMinimalPos;
		for (auto i = 0; i < getAttachmentCount(); i++) 
		{
			auto attach = getAttachment(i);
			vec3 hitInWorld;
			if(attach->isHit(ray, hitInWorld)) 
			{
				count += 1;
				if (hitInWorld.distance(ray.origin()) < minDist) 
				{
					resultIndx = i;
					minDist = hitInWorld.distance(ray.origin());
					theMinimalPos = hitInWorld;
				}
	        }
		}
		if (resultIndx >= 0) 
		{
			auto attachPtr = getAttachmentInfo(resultIndx, attachPosition, Normal, up);
			//DebugSystem::shared()->drawPointCross(theMinimalPos);
			return attachPtr;
		}
		return nullptr;
	}

	void GamePart::AddOnVehicle(Vehicle* vehicle)
	{
		
	}

	void GamePart::setSurface(vec3 color, PartSurface* surface)
	{
		m_partSurface = surface;
		getNode()->setColor(vec4(color, 1.0));
		
		if(getItem()->m_visualInfo.type != VisualInfo::VisualInfoType::Mesh)
		{
			auto mat = getNode()->getMaterial();
			auto texture =  TextureMgr::shared()->getByPath(surface->getDiffusePath());
			mat->setTex("DiffuseMap", texture);

			auto metallicTexture =  TextureMgr::shared()->getByPath(surface->getMetallicPath());
			mat->setTex("MetallicMap", metallicTexture);

			auto roughnessTexture =  TextureMgr::shared()->getByPath(surface->getRoughnessPath());
			mat->setTex("RoughnessMap", roughnessTexture);

			auto normalMapTexture =  TextureMgr::shared()->getByPath(surface->getNormalMapPath());
			mat->setTex("NormalMap", normalMapTexture);
		}
	}

	void GamePart::attachTo(Attachment * attach, float degree, int attachMentIndex)
	{
		vec3 attachPosition,  Normal,  up;
		attach->getAttachmentInfo(attachPosition, Normal, up);
		//we use m_attachment[0]
		Normal = Normal * -1;
		auto selfAttah = getAttachment(attachMentIndex);
        selfAttah->m_connected = attach;
        attach->m_connected = selfAttah;
		vec3 right = vec3::CrossProduct(Normal, up);
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

		data[8] = -Normal.x;
		data[9] = -Normal.y;
		data[10] = -Normal.z;
		data[11] = 0.0;

		data[12] = attachPosition.x;
		data[13] = attachPosition.y;
		data[14] = attachPosition.z;
		data[15] = 1.0;


		Matrix44 selfAttachmentTrans;
		data = selfAttachmentTrans.data();
		auto rightForAttach = vec3::CrossProduct(selfAttah->m_normal, selfAttah->m_up);
		data[0] = rightForAttach.x;
		data[1] = rightForAttach.y;
		data[2] = rightForAttach.z;
		data[3] = 0.0;

		data[4] = selfAttah->m_up.x;
		data[5] = selfAttah->m_up.y;
		data[6] = selfAttah->m_up.z;
		data[7] = 0.0;

		//use invert
		data[8] = -selfAttah->m_normal.x;
		data[9] = -selfAttah->m_normal.y;
		data[10] = -selfAttah->m_normal.z;
		data[11] = 0.0;

		data[12] = selfAttah->m_pos.x;
		data[13] = selfAttah->m_pos.y;
		data[14] = selfAttah->m_pos.z;
		data[15] = 1.0;

		Matrix44 rotateMatrix;
		Quaternion qRotate;
		qRotate.fromEulerAngle(vec3(0, 0, degree));
		rotateMatrix.setRotation(qRotate);
		selfAttachmentTrans = selfAttachmentTrans * rotateMatrix;
		auto result = transformForAttachPoint * selfAttachmentTrans.inverted();
		Quaternion q;
		q.fromRotationMatrix(&result);
		m_node->setPos(result.getTranslation());
		m_node->setRotateQ(q);
	}

	Matrix44 GamePart::attachToOtherIslandByAlterSelfPart(Attachment * attach, int attachmentIndex)
	{
		auto selfAttah = getAttachment(attachmentIndex);
        if (attach->m_parent->isConstraint()) 
		{
			static_cast<GameConstraint *>(attach->m_parent)->m_a = selfAttah;
        }
        selfAttah->m_connected = attach;
        attach->m_connected = selfAttah;
		if(!selfAttah->m_connected->m_parent->m_parent) 
		{
			tlog("wrong");
		}
		if(!attach->m_connected->m_parent->m_parent) 
		{
			tlog("wrong");
		}
		return adjustToOtherIslandByAlterSelfPart(attach, selfAttah, 0);
	}

	Matrix44 GamePart::attachToOtherIslandByAlterSelfIsland(Attachment* attach, Attachment * ownAttachment, float degree)
	{
		Attachment * selfAttah = nullptr;
		//we use m_attachment[0]
		if (!ownAttachment) 
		{
			selfAttah = getFirstAttachment();  
		}
		else 
		{
            selfAttah = ownAttachment;
		}
        selfAttah->m_connected = attach;
		selfAttah->m_degree = degree;
		attach->m_connected = selfAttah;
		attach->m_degree = -1.0f * degree;
		if(attach->m_parent->isConstraint())
		{
			static_cast<GameConstraint *>(attach->m_parent)->m_a = selfAttah;
		}
		return adjustToOtherIslandByAlterSelfIsland(attach, selfAttah, degree);
	}

	Matrix44 GamePart::adjustToOtherIslandByAlterSelfIsland(Attachment* attach, Attachment* selfAttah, float degree)
	{
		vec3 attachPosition,  Normal,  up;
		attach->getAttachmentInfoWorld(attachPosition, Normal, up);
		vec3 InvertedNormal = Normal * -1;
		attachPosition = attachPosition + Normal * 0.01f;

		vec3 right = vec3::CrossProduct(InvertedNormal, up);
		Matrix44 attachOuterWorldMat;
		auto data = attachOuterWorldMat.data();
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


		Matrix44 selfAttachmentTrans;
		data = selfAttachmentTrans.data();
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


		Matrix44 rotateMatrix;
		Quaternion qRotate;
		qRotate.fromEulerAngle(vec3(0, 0, degree));
		rotateMatrix.setRotation(qRotate);
		selfAttachmentTrans = selfAttachmentTrans * rotateMatrix;

		auto result = attachOuterWorldMat * selfAttachmentTrans.inverted() * getNode()->getLocalTransform().inverted();

		Quaternion q;
		q.fromRotationMatrix(&result);
		m_parent->m_node->setPos(result.getTranslation());
		m_parent->m_node->setRotateQ(q);
		m_parent->m_node->reCache();
		return result;
	}

	Matrix44 GamePart::adjustToOtherIslandByAlterSelfPart(Attachment* attach, Attachment* selfAttah, float degree)
	{
		vec3 attachPosition,  Normal,  up;
		attach->getAttachmentInfoWorld(attachPosition, Normal, up);
		vec3 InvertedNormal = Normal * -1;
		attachPosition = attachPosition + Normal * 0.01f;

		vec3 right = vec3::CrossProduct(InvertedNormal, up);
		Matrix44 attachOuterWorldMat;
		auto data = attachOuterWorldMat.data();
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

		
		Matrix44 selfAttachmentTrans;
		data = selfAttachmentTrans.data();
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


		Matrix44 rotateMatrix;
		Quaternion qRotate;
		qRotate.fromEulerAngle(vec3(0, 0, degree));
		rotateMatrix.setRotation(qRotate);
		selfAttachmentTrans = selfAttachmentTrans * rotateMatrix;

		auto result = m_parent->m_node->getLocalTransform().inverted() * attachOuterWorldMat * selfAttachmentTrans.inverted();
		Quaternion q;
		q.fromRotationMatrix(&result);
		m_node->setPos(result.getTranslation());
		m_node->setRotateQ(q);
		m_node->reCache();
		return result;
	}

	Attachment* GamePart::getFirstAttachment()
	{
		return m_attachment[0];
	}

	Attachment* GamePart::getBottomAttachment()
	{
		int theSmallIndex = -1;
		float smallDist = 99999.0f;
		for(int i =0; i < getAttachmentCount(); i++) 
		{
			vec3 pos, n, up;
			auto attach = getAttachment(i);
			attach->getAttachmentInfoWorld(pos, n, up);
			if(pos.y < smallDist) 
			{
				smallDist = pos.y;
				theSmallIndex = i;
			}
		}
		return m_attachment[theSmallIndex];
	}

	Attachment* GamePart::getTopAttachment()
	{
		return getFirstAttachment();
	}

	Attachment* GamePart::getAttachment(int index)
	{
		return m_attachment[index];
	}

	int GamePart::getAttachmentCount()
	{
		return m_attachment.size();
	}

	Attachment* GamePart::getAttachmentInfo(int index, vec3& pos, vec3& N, vec3& up)
	{
		auto mat = m_node->getLocalTransform();
		auto atta = getAttachment(index);
		vec4 a_pos = vec4(atta->m_pos, 1.0);
		vec4 a_n = vec4(atta->m_normal, 0.0);
		vec4 a_up = vec4(atta->m_up, 0.0);
		pos = mat.transofrmVec4(a_pos).toVec3();
		N = mat.transofrmVec4(a_n).toVec3();
		up = mat.transofrmVec4(a_up).toVec3();
		return atta;
	}

	GamePart::GamePart()
	{
		m_graphNode = nullptr;
		m_name = "empty";
		m_partSurface = PartSurfaceMgr::shared()->getItem("Metal Grid3");
	}

	GamePart::~GamePart()
	{	
		m_node->removeFromParent();
		delete m_node;
		for(auto attach : m_attachment)
		{
			delete attach;
		}
		m_attachment.clear();
	}

	GamePartType GamePart::getType()
	{
		return GamePartType::GAME_PART_NOT_VALID;
	}

	float GamePart::getMass()
	{
		if(m_item)
		{
			return m_item->m_physicsInfo.mass;
		}
		return 0.25f;
	}

	void GamePart::load(rapidjson::Value& partData)
	{
		setGUID(partData["UID"].GetString());
		getNode()->setPos(vec3(partData["pos"][0].GetDouble(),partData["pos"][1].GetDouble(), partData["pos"][2].GetDouble()));
		auto q = Quaternion(partData["rotate"][0].GetDouble(),partData["rotate"][1].GetDouble(), partData["rotate"][2].GetDouble(), partData["rotate"][3].GetDouble());
		getNode()->setRotateQ(q);

		loadAttach(partData);

		if(partData.HasMember("Name"))
		{
			m_name = partData["Name"].GetString();
		}

		auto surface = PartSurfaceMgr::shared()->getItem(partData["PartSurface"].GetString());
		vec3 color(partData["Color"][0].GetDouble(),partData["Color"][1].GetDouble(),partData["Color"][2].GetDouble());

		//apply surface color
		setSurface(color, surface);
	}

	void GamePart::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		//pos
		auto pos = getNode()->getPos();
		partDocObj.AddMember("UID", std::string(getGUID()), allocator);
		rapidjson::Value posList(rapidjson::kArrayType);
		posList.PushBack(pos.x, allocator).PushBack(pos.y, allocator).PushBack(pos.z, allocator);
		partDocObj.AddMember("pos", posList, allocator);
		

		//rotate
		auto rotate = getNode()->getRotateQ();
		rapidjson::Value rotateList(rapidjson::kArrayType);
		rotateList.PushBack(rotate.x, allocator).PushBack(rotate.y, allocator).PushBack(rotate.z, allocator).PushBack(rotate.w, allocator);

		partDocObj.AddMember("rotate", rotateList, allocator);

		//type
		partDocObj.AddMember("type", int(getType()), allocator);

		partDocObj.AddMember("Name", m_name, allocator);

		partDocObj.AddMember("ItemName", m_item->m_name, allocator);

		partDocObj.AddMember("PartSurface", m_partSurface->getName(), allocator);

		auto color = getNode()->getColor();
		rapidjson::Value colorList(rapidjson::kArrayType);
		colorList.PushBack(color.x, allocator).PushBack(color.y, allocator).PushBack(color.z, allocator);

		partDocObj.AddMember("Color", colorList, allocator);
		dumpAttach(partDocObj, allocator);
	}

	bool GamePart::isConstraint()
	{
		return false;
	}

	vec3 GamePart::getWorldPos()
	{
		return m_node->getWorldPos();
	}

	std::string GamePart::getName() const
	{
		return m_name;
	}

	void GamePart::setName(std::string newName)
	{
		m_name = newName;
	}

	void GamePart::generateName()
	{
	}

	static char randomChar()
	{
		char randomList[] = {'A', 'B', 'C', 'D', 'E', 'F', '1', '2', '3','4','5','6','7','8','9'};
		return randomList[rand() % sizeof(randomList)];
		
	}
	std::string GamePart::genShortName()
	{
		//4 char short random name
		char formatName[512];
		sprintf_s(formatName, 512, u8"%c%c%c%c",randomChar(), randomChar(), randomChar(), randomChar());
		return formatName;
	}

	void GamePart::highLight()
	{
		if(m_node)
		{
			m_node->setColor(vec4(1.0, 0.5, 0.5, 1.0));
		}
	}

	void GamePart::unhighLight()
	{
		if(m_node)
		{
			m_node->setColor(vec4(1.0, 1.0, 1.0, 1.0));
		}
	}

	void GamePart::use()
	{
	}

	void GamePart::dumpAttach(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value attachList(rapidjson::kArrayType);
		int count = getAttachmentCount();
		for(int k = 0; k < count; k++)
		{
			auto attach = getAttachment(k);
			rapidjson::Value attachObj(rapidjson::kObjectType);
			attachObj.AddMember("UID", std::string(attach->getGUID()), allocator);
			std::string UID = "empty";
            if (attach->m_connected) 
			{
            	UID = attach->m_connected->getGUID();
            }
			attachObj.AddMember("to", std::string(UID), allocator);
			attachObj.AddMember("degree", attach->m_degree, allocator);
			attachList.PushBack(attachObj, allocator);
		}
		partDocObj.AddMember("attachList", attachList, allocator);
	}

	void GamePart::loadAttach(rapidjson::Value& partData)
	{
		if (partData.HasMember("attachList"))
		{
			auto& attachList = partData["attachList"];
			for(unsigned int i =0; i < attachList.Size(); i ++)
			{
				auto &attachData = attachList[i];
				std::string guid_str = attachData["UID"].GetString();
				getAttachment(i)->setGUID(guid_str.c_str());
				if(attachData.HasMember("to"))
				{
					getAttachment(i)->m_connectedGUID = attachData["to"].GetString();
				}
				if(attachData.HasMember("degree"))
				{
					getAttachment(i)->m_degree = attachData["degree"].GetDouble();
				} else
				{
					getAttachment(i)->m_degree = 0.0f;
				}
				getAttachment(i)->m_parent = this;
			}
        }
	}

	void GamePart::addAttachment(Attachment* newAttach)
	{
		m_attachment.push_back(newAttach);
	}

	bool GamePart::isHit(Ray ray)
	{
		auto island = m_parent;
		auto node = getNode();
		auto invertedMat = node->getTransform().inverted();
		vec4 dirInLocal = invertedMat * vec4(ray.direction(), 0.0);
		vec4 originInLocal = invertedMat * vec4(ray.origin(), 1.0);

		auto r = Ray(originInLocal.toVec3(), dirInLocal.toVec3());
		RayAABBSide side;
		vec3 hitPoint;
		auto isHit = r.intersectAABB(node->localAABB(), &side, hitPoint);
		GamePart* newPart = nullptr;
		if (isHit)
		{
			return true;
		}
		return false;
	}

	void GamePart::drawInspect()
	{
		
	}

	bool GamePart::isNeedDrawInspect()
	{
		return false;
	}

	bool GamePart::drawInspectNameEdit()
	{
		char a[128] = "";
		strcpy(a, getName().c_str());
		ImGui::PushItemWidth(80);
		bool isInputName = ImGui::InputText(u8"Ãû³Æ",a,128);
		ImGui::PopItemWidth();
		if(isInputName)
		{
			setName(a);
			return true;
		}
		return false;
	}

	void GamePart::initFromItemName(std::string itemName)
	{
		auto item = ItemMgr::shared()->getItem(itemName);
		initFromItem(item);
	}

	void GamePart::initFromItem(GameItem* item)
	{
		m_item = item;
		//material
	    auto m_material = Material::createFromTemplate("ModelPBR");
		auto texture =  TextureMgr::shared()->getByPath("Texture/metalgrid3-ue/metalgrid3_basecolor.png", true);
		m_material->setTex("DiffuseMap", texture);

		auto metallicTexture =  TextureMgr::shared()->getByPath("Texture/metalgrid3-ue/metalgrid3_metallic.png", true);
		m_material->setTex("MetallicMap", metallicTexture);

		auto roughnessTexture =  TextureMgr::shared()->getByPath("Texture/metalgrid3-ue/metalgrid3_roughness.png", true);
		m_material->setTex("RoughnessMap", roughnessTexture);


		auto normalMapTexture =  TextureMgr::shared()->getByPath("Texture/metalgrid3-ue/metalgrid3_normal-dx.png", true);
		m_material->setTex("NormalMap", normalMapTexture);
		bool isNeedSetDefaultMat = false;
		//visual part
		switch(item->m_visualInfo.type)
		{
		case VisualInfo::VisualInfoType::CubePrimitive:
		{
			auto size = item->m_visualInfo.size;
			m_node = new CubePrimitive(size.x, size.y, size.z, false);
			isNeedSetDefaultMat = true;
		}
		break;
		case VisualInfo::VisualInfoType::CylinderPrimitive:
		{
			auto size = item->m_visualInfo.size;
			m_node = new CylinderPrimitive(size.x, size.y, size.z);
			isNeedSetDefaultMat = true;
		}
		break;
		case VisualInfo::VisualInfoType::Mesh:
		{
			auto size = item->m_visualInfo.size;
			//sorry not supported yet
			auto model = Model::create(item->m_visualInfo.filePath);
			auto tex = TextureMgr::shared()->getByPath(item->m_visualInfo.diffusePath, true);
			model->getMat(0)->setTex("DiffuseMap", tex);
			model->getMat(0)->setTex("RoughnessMap", TextureMgr::shared()->getByPath(item->m_visualInfo.roughnessPath, true));
			model->getMat(0)->setTex("MetallicMap", TextureMgr::shared()->getByPath(item->m_visualInfo.metallicPath, true));
			model->getMat(0)->setTex("NormalMap", TextureMgr::shared()->getByPath(item->m_visualInfo.normalMapPath, true));
			model->setScale(size.x, size.y, size.z);
			m_node = model;
			isNeedSetDefaultMat = false;
			for(int i = 0; i < item->m_visualInfo.extraFileList.size(); i++)
			{
				auto modelExtra = Model::create(item->m_visualInfo.extraFileList[i]);
				model->addExtraMeshList(modelExtra->getMeshList());
			}
		}
		break;
			
        case VisualInfo::VisualInfoType::RightPrismPrimitive:
		{
			auto size = item->m_visualInfo.size;
			m_node = new RightPrismPrimitive(size.x, size.y, size.z);
			isNeedSetDefaultMat = true;
        }
		break;
		default: ;
		}
		if(isNeedSetDefaultMat)
		{
			m_node->setMaterial(m_material);
		}
		

		//Physics Part
		m_shape = new PhysicsShape();
		switch(item->m_physicsInfo.type)
		{
		case PhysicsInfo::PhysicsInfoType::BoxShape:
		{
			auto size = item->m_physicsInfo.size;
			m_shape->initBoxShape(vec3(size.x, size.y, size.z));
		}
		break;
	    case PhysicsInfo::PhysicsInfoType::CylinderShape:
		{
			auto size = item->m_physicsInfo.size;
			m_shape->initCylinderShapeZ(size.x, size.y, size.z);
		}
		break;
		default: ;
		}
		for(int i = 0; i < item->m_attachList.size(); i++)
		{
			auto attach = item->m_attachList[i];
			auto newAttach = new Attachment(attach.pos, attach.normal, attach.up ,this);
			newAttach->m_locale = attach.locale;
			newAttach->m_collisionSize = attach.collisionSize;
			addAttachment(newAttach);
			//auto cube = new CubePrimitive(newAttach->m_collisionSize, newAttach->m_collisionSize, 0.5);
			//Quaternion q;
			//newAttach->getAttachmentInfoMat44().getRotation(&q);
			//cube->setPos(attach.pos);
			//cube->setRotateQ(q);
			////cube->setRotateE(90, 90, 0);
			//m_node->addChild(cube);
		}
		//set Part Surface
		setSurface(item->m_tintColor, PartSurfaceMgr::shared()->getItem(item->m_surfaceName));
	}

	int GamePart::getPrettyAttach(Attachment* otherAttach, int attachOffset)
	{
		auto aLocale = otherAttach->m_locale;
		std::string targetLocale = "down";
		if(aLocale == "up")
		{
			targetLocale = "down";
		}
		else if(aLocale == "down")
		{
			targetLocale = "up";
		}
		else if(aLocale == "left")
		{
			targetLocale = "right";
		}
		else if(aLocale == "right")
		{
			targetLocale = "left";
		}
		else if(aLocale == "front")
		{
			targetLocale = "back";
		}
		else if(aLocale == "back")
		{
			targetLocale = "front";
		}
		int targetIndex = findAttachByLocale(targetLocale);

		return (targetIndex + attachOffset) % (m_attachment.size());
	}

	int GamePart::findAttachByLocale(std::string locale)
	{
		for(int i = 0; i< m_attachment.size(); i++)
		{
			auto attach = m_attachment[i];
			if(attach->m_locale == locale)
			{
				return i;
			}
		}
		return 0;
	}

	void GamePart::toggle(int openSignal)
	{
		
	}

	void GamePart::setVehicle(Vehicle* vehicle)
	{
		m_vehicle = vehicle;
		AddOnVehicle(vehicle);
	}

	Vehicle* GamePart::getVehicle()
	{
		return m_vehicle;
	}

	GraphNode* GamePart::getGraphNode() const
	{
		return m_graphNode;
	}


	GameItem* GamePart::getItem() const
	{
		return m_item;
	}

	void GamePart::setItem(GameItem* const item)
	{
		m_item = item;
	}
}
