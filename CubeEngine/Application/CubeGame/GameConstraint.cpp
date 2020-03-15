#include "GameConstraint.h"
#include "GamePart.h"
#include "2D/LabelNew.h"
#include "Scene/SceneMgr.h"
#include "GameItem.h"

namespace tzw
{
GameConstraint::GameConstraint()
{
	m_a = nullptr;
	m_b = nullptr;
	m_isEnablePhysics = false;
}

void GameConstraint::enablePhysics(bool isEnable)
{
	m_isEnablePhysics = isEnable;
}

bool GameConstraint::isEnablePhysics()
{
	return m_isEnablePhysics;
}

void GameConstraint::dump(rapidjson::Value& partData, rapidjson::Document::AllocatorType& allocator)
{
	partData.AddMember("Name", m_name, allocator);
	partData.AddMember("ItemName", m_item->m_name, allocator);
}

void GameConstraint::load(rapidjson::Value& partData)
{
	
}

bool GameConstraint::isConstraint()
{
	return true;
}

void GameConstraint::updateTransform(float dt)
{
	if(m_isEnablePhysics)
	{
		onUpdate(dt);
		for(int i = 0; i < getAttachmentCount(); i++)
		{
			auto attach = getAttachment(i);
			if(attach->m_connected)
			{
				adjustToOtherIslandByAlterSelfIsland(attach->m_connected, attach, attach->m_degree);
				break;
			}
		}
	}
}

void GameConstraint::setName(std::string newName)
{
	GamePart::setName(newName);
}

GameConstraint::~GameConstraint()
{
}

void GameConstraint::updateConstraintState()
{
	
}

void GameConstraint::onUpdate(float dt)
{
}

Matrix44 GameConstraint::setUpFrameFromZ(vec3 pos, vec3 Zaixs, Matrix44 reservedMat)
{
	Matrix44 mat;
	vec3 refY(0, 1, 0);
	//swap the axis if the z and y is too closed.
	if(vec3::DotProduct(refY, Zaixs) > 0.98)
	{
		refY = vec3(0, 0, 1);
	}
	auto data = mat.data();
	vec3 right = vec3::CrossProduct(refY, Zaixs).normalized();

	vec3 trueUp = vec3::CrossProduct(Zaixs, right).normalized();
	data[0] = right.x;
	data[1] = right.y;
	data[2] = right.z;
	data[3] = 0.0;

	data[4] = trueUp.x;
	data[5] = trueUp.y;
	data[6] = trueUp.z;
	data[7] = 0.0;

	data[8] = Zaixs.x;
	data[9] = Zaixs.y;
	data[10] = Zaixs.z;
	data[11] = 0.0;

	data[12] = pos.x;
	data[13] = pos.y;
	data[14] = pos.z;
	data[15] = 1.0;

	return reservedMat * mat;
}
}

