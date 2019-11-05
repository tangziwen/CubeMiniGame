#include "GamePart.h"
#include "Island.h"
#include "BearPart.h"
#include "2D/GUISystem.h"

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
		for (auto i = 0; i < getAttachmentCount(); i++) 
		{
			auto attach = getAttachment(i);
			vec3 hitInWorld;
			if(attach->isHit(ray, hitInWorld)) 
			{
				if (hitInWorld.distance(ray.origin()) < minDist) 
				{
					resultIndx = i;
					minDist = hitInWorld.distance(ray.origin());
				}
	        }
		}
		if (resultIndx >= 0) 
		{
			auto attachPtr = getAttachmentInfo(resultIndx, attachPosition, Normal, up);
			return attachPtr;
		}
		return nullptr;
	}

	void GamePart::attachTo(Attachment * attach, float degree)
	{
		vec3 attachPosition,  Normal,  up;
		attach->getAttachmentInfo(attachPosition, Normal, up);
		//we use m_attachment[0]
		Normal = Normal * -1;
		auto selfAttah = getFirstAttachment();
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
		rotateMatrix.rotate(qRotate);
		selfAttachmentTrans = selfAttachmentTrans * rotateMatrix;
		auto result = transformForAttachPoint * selfAttachmentTrans.inverted();
		Quaternion q;
		q.fromRotationMatrix(&result);
		m_node->setPos(result.getTranslation());
		m_node->setRotateQ(q);
	}

	Matrix44 GamePart::attachToOtherIslandByAlterSelfPart(Attachment * attach)
	{
		auto selfAttah = getFirstAttachment();
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
		return adjustToOtherIslandByAlterSelfPart(attach, selfAttah);
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
		rotateMatrix.rotate(qRotate);
		selfAttachmentTrans = selfAttachmentTrans * rotateMatrix;

		auto result = attachOuterWorldMat * selfAttachmentTrans.inverted() * getNode()->getLocalTransform().inverted();

		Quaternion q;
		q.fromRotationMatrix(&result);
		m_parent->m_node->setPos(result.getTranslation());
		m_parent->m_node->setRotateQ(q);
		m_parent->m_node->reCache();

		return result;
	}

	Matrix44 GamePart::adjustToOtherIslandByAlterSelfPart(Attachment* attach, Attachment* selfAttach)
	{
		auto islandMatrixInverted = m_parent->m_node->getLocalTransform().inverted();
		vec3 attachPosition,  Normal,  up;
		attach->getAttachmentInfoWorld(attachPosition, Normal, up);
		//transform other island attachment to our island
		attachPosition = islandMatrixInverted.transformVec3(attachPosition);
		Normal = islandMatrixInverted.transofrmVec4(vec4(Normal, 0.0)).toVec3();
		vec3 InvertedNormal = Normal * -1;
		attachPosition = attachPosition + Normal * 0.01f;
		up = islandMatrixInverted.transofrmVec4(vec4(up, 0.0)).toVec3();
		//we use m_attachment[0]

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
		auto rightForAttach = vec3::CrossProduct(selfAttach->m_normal, selfAttach->m_up);
		vec3 normalForAttach = selfAttach->m_normal;
		data[0] = rightForAttach.x;
		data[1] = rightForAttach.y;
		data[2] = rightForAttach.z;
		data[3] = 0.0;

		data[4] = selfAttach->m_up.x;
		data[5] = selfAttach->m_up.y;
		data[6] = selfAttach->m_up.z;
		data[7] = 0.0;

		//use invert
		data[8] = -normalForAttach.x;
		data[9] = -normalForAttach.y;
		data[10] = -normalForAttach.z;
		data[11] = 0.0;

		data[12] = selfAttach->m_pos.x;
		data[13] = selfAttach->m_pos.y;
		data[14] = selfAttach->m_pos.z;
		data[15] = 1.0;

		auto result = transformForAttachPoint * attachmentTrans.inverted();
		Quaternion q;
		q.fromRotationMatrix(&result);
		m_node->setPos(result.getTranslation());
		m_node->setRotateQ(q);
		m_node->reCache();
		return result;
	}

	Attachment* GamePart::getFirstAttachment()
	{
		return nullptr;
	}

	Attachment* GamePart::getBottomAttachment()
	{
		return getFirstAttachment();
	}

	Attachment* GamePart::getTopAttachment()
	{
		return getFirstAttachment();
	}

	Attachment* GamePart::getAttachment(int index)
	{
		return nullptr;
	}

	int GamePart::getAttachmentCount()
	{
		return 0;
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
		m_name = "empty";
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
		return 1.0f;
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

	void GamePart::drawInspectNameEdit()
	{
		char a[128] = "";
		strcpy(a, getName().c_str());
		ImGui::PushItemWidth(80);
		bool isInputName = ImGui::InputText(u8"Ãû³Æ",a,128);
		ImGui::PopItemWidth();
		if(isInputName)
		{
			setName(a);
		}
	}
}
