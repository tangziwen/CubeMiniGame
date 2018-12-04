#include "BlockPart.h"
#include "3D/Primitive/CubePrimitive.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"

namespace tzw
{
const float blockSize = 0.5;
BlockPart::BlockPart()
{
	m_node = new CubePrimitive(blockSize, blockSize, blockSize);
	m_shape = new PhysicsShape();
	m_shape->initBoxShape(vec3(blockSize, blockSize, blockSize));
	m_parent = nullptr;
	for(int i = 0; i < 6; i++)
	{
		m_bearPart[i] = nullptr;
	}
	initAttachments();
}

bool BlockPart::findProperAttachPoint(Ray ray, vec3 &attachPosition, vec3 &Normal, vec3 & attachUp)
{
	RayAABBSide side;
	vec3 hitPoint;
	auto isHit = ray.intersectAABB(m_node->localAABB(), &side, hitPoint);

	if (!isHit) return false;


	auto m = m_node->getLocalTransform().data();
	vec3 up(m[4], m[5], m[6]);
	vec3 forward(-m[8], -m[9], -m[10]);
	vec3 right(m[0], m[1], m[2]);
	up.normalize();
	forward.normalize();
	right.normalize();
	switch (side)
	{
	case RayAABBSide::up:
	{
		getAttachmentInfo(4, attachPosition, Normal, attachUp);
	}
	break;
	case RayAABBSide::down:
	{
		getAttachmentInfo(5, attachPosition, Normal, attachUp);
	}
	break;
	case RayAABBSide::left:
	{
		getAttachmentInfo(3, attachPosition, Normal, attachUp);
	}
	break;
	case RayAABBSide::right:
	{
		getAttachmentInfo(2, attachPosition, Normal, attachUp);
	}
	break;
	case RayAABBSide::front:
	{
		getAttachmentInfo(0, attachPosition, Normal, attachUp);
	}
	break;
	case RayAABBSide::back:
	{
		getAttachmentInfo(1, attachPosition, Normal, attachUp);
	}
	break;
	default:
		break;
	}
	return true;
}

void BlockPart::attachTo(vec3 attachPosition, vec3 Normal, vec3 up)
{
	//we use m_attachment[0]
	auto atta = m_attachment[0];
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
	auto rightForAttach = vec3::CrossProduct(atta.m_normal, atta.m_up); 
	data[0] = rightForAttach.x;
	data[1] = rightForAttach.y;
	data[2] = rightForAttach.z;
	data[3] = 0.0;

	data[4] = atta.m_up.x;
	data[5] = atta.m_up.y;
	data[6] = atta.m_up.z;
	data[7] = 0.0;

	data[8] = atta.m_normal.x;
	data[9] = atta.m_normal.y;
	data[10] = atta.m_normal.z;
	data[11] = 0.0;

	data[12] = atta.m_pos.x;
	data[13] = atta.m_pos.y;
	data[14] = atta.m_pos.z;
	data[15] = 1.0;

	auto result = transformForAttachPoint * attachmentTrans;
	Quaternion q;
	q.fromRotationMatrix(&result);
	m_node->setPos(result.getTranslation());
	m_node->setRotateQ(q);
}

void BlockPart::initAttachments()
{
	//forward backward
	m_attachment[0] = Attachment(vec3(0.0, 0.0, blockSize / 2.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0));
	m_attachment[1] = Attachment(vec3(0.0, 0.0, -blockSize / 2.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0));

	//right left
	m_attachment[2] = Attachment(vec3(blockSize / 2.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
	m_attachment[3] = Attachment(vec3(-blockSize / 2.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));

	//up down
	m_attachment[4] = Attachment(vec3(0.0, blockSize / 2.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));
	m_attachment[5] = Attachment(vec3(0.0, -blockSize / 2.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, 1.0));
}

void BlockPart::getAttachmentInfo(int index, vec3 & pos, vec3 & N, vec3 & up)
{
	auto mat = m_node->getLocalTransform();
	auto atta = m_attachment[index];
	vec4 a_pos = vec4(atta.m_pos, 1.0);
	vec4 a_n = vec4(atta.m_normal, 0.0);
	vec4 a_up = vec4(atta.m_up, 0.0);
	pos = mat.transofrmVec4(a_pos).toVec3();
	N = mat.transofrmVec4(a_n).toVec3();
	up = mat.transofrmVec4(a_up).toVec3();
}

void BlockPart::cook()
{
	auto mat2 = m_node->getTranslationMatrix();
	auto aabb = m_node->getAABB();
	auto rigChasis = PhysicsMgr::shared()->createRigidBody(1.0, mat2, aabb);
	rigChasis->attach(m_node);
}

Attachment::Attachment(vec3 thePos, vec3 n, vec3 up)
{
	m_pos = thePos;
	m_normal = n;
	m_up = up;
}

Attachment::Attachment()
{
}

}
