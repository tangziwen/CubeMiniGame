#include "GamePart.h"
#include "Island.h"
#include "BearPart.h"

namespace tzw
{

	Attachment::Attachment(vec3 thePos, vec3 n, vec3 up, GamePart * parent)
	{
		m_pos = thePos;
		m_normal = n;
		m_up = up;
		m_bearPart = nullptr;
		m_parent = parent;
	}

	Attachment::Attachment()
	{
		m_bearPart = nullptr;
	}

	void Attachment::getAttachmentInfo(vec3 & pos, vec3 & N, vec3 & up)
	{
		auto mat = m_parent->getNode()->getLocalTransform();
		vec4 a_pos = vec4(m_pos, 1.0);
		vec4 a_n = vec4(m_normal, 0.0);
		vec4 a_up = vec4(m_up, 0.0);
		pos = mat.transofrmVec4(a_pos).toVec3();
		N = mat.transofrmVec4(a_n).toVec3();
		up = mat.transofrmVec4(a_up).toVec3();
	}

	void Attachment::getAttachmentInfoWorld(vec3 & pos, vec3 & N, vec3 & up)
	{
		auto mat = m_parent->getNode()->getTransform();
		vec4 a_pos = vec4(m_pos, 1.0);
		vec4 a_n = vec4(m_normal, 0.0);
		vec4 a_up = vec4(m_up, 0.0);
		pos = mat.transofrmVec4(a_pos).toVec3();
		N = mat.transofrmVec4(a_n).toVec3();
		up = mat.transofrmVec4(a_up).toVec3();
	}

	Matrix44 Attachment::getAttachmentInfoMat44()
	{
		vec3 right = vec3::CrossProduct(m_normal, m_up);
		Matrix44 transformForAttachPoint;
		auto data = transformForAttachPoint.data();
		data[0] = right.x;
		data[1] = right.y;
		data[2] = right.z;
		data[3] = 0.0;

		data[4] = m_up.x;
		data[5] = m_up.y;
		data[6] = m_up.z;
		data[7] = 0.0;
		//right hand
		data[8] = -m_normal.x;
		data[9] = -m_normal.y;
		data[10] = -m_normal.z;
		data[11] = 0.0;

		data[12] = m_pos.x;
		data[13] = m_pos.y;
		data[14] = m_pos.z;
		data[15] = 1.0;
		return transformForAttachPoint;
	}

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
		return false;
	}

	void GamePart::attachTo(Attachment * attach)
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


		Matrix44 attachmentTrans;
		data = attachmentTrans.data();
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

		auto result = transformForAttachPoint * attachmentTrans.inverted();
		Quaternion q;
		q.fromRotationMatrix(&result);
		m_node->setPos(result.getTranslation());
		m_node->setRotateQ(q);
	}

	Matrix44 GamePart::attachToFromOtherIsland(Attachment * attach, BearPart * bearing)
	{
		auto islandMatrixInverted = m_parent->m_node->getLocalTransform().inverted();
		vec3 attachPosition,  Normal,  up;
		attach->getAttachmentInfoWorld(attachPosition, Normal, up);
		//transform other island attachment to our island
		attachPosition = islandMatrixInverted.transformVec3(attachPosition);
		Normal = islandMatrixInverted.transofrmVec4(vec4(Normal, 0.0)).toVec3();
		vec3 InvertedNormal = Normal * -1;
		attachPosition = attachPosition + Normal * 0.01;
		up = islandMatrixInverted.transofrmVec4(vec4(up, 0.0)).toVec3();
		//we use m_attachment[0]
		auto selfAttah = getFirstAttachment();
		if (bearing)
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
		return result;
	}

	Matrix44 GamePart::attachToFromOtherIslandAlterSelfIsland(Attachment* attach, BearPart* bearing)
	{
		auto islandMatrixInverted = m_parent->m_node->getLocalTransform().inverted();
		vec3 attachPosition,  Normal,  up;
		attach->getAttachmentInfoWorld(attachPosition, Normal, up);
		//transform other island attachment to our island
		attachPosition = islandMatrixInverted.transformVec3(attachPosition);
		Normal = islandMatrixInverted.transofrmVec4(vec4(Normal, 0.0)).toVec3();
		vec3 InvertedNormal = Normal * -1;
		attachPosition = attachPosition + Normal * 0.01;
		up = islandMatrixInverted.transofrmVec4(vec4(up, 0.0)).toVec3();
		//we use m_attachment[0]
		auto selfAttah = getFirstAttachment();
		if (bearing)
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
		return result;
	}

	Attachment* GamePart::getFirstAttachment()
	{
		return nullptr;
	}

	GamePart::~GamePart()
	{
		m_node->removeFromParent();
		delete m_node;
	}

	int GamePart::getType()
	{
		return GAME_PART_NOT_VALID;
	}

	float GamePart::getMass()
	{
		return 1.0f;
	}
}
