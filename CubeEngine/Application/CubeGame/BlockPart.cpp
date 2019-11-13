#include "BlockPart.h"
#include "3D/Primitive/CubePrimitive.h"
#include "Scene/SceneMgr.h"
#include "Collision/PhysicsMgr.h"
#include "Island.h"
#include "ItemMgr.h"
#include "3D/Primitive/CylinderPrimitive.h"

namespace tzw
{
const float blockSize = 0.5;
BlockPart::BlockPart()
{
	m_node = new CubePrimitive(blockSize, blockSize, blockSize, false);

    auto m_material = Material::createFromTemplate("ModelPBR");
	auto texture =  TextureMgr::shared()->getByPath("Texture/metalgrid3-ue/metalgrid3_basecolor.png");
	m_material->setTex("DiffuseMap", texture);

	auto metallicTexture =  TextureMgr::shared()->getByPath("Texture/metalgrid3-ue/metalgrid3_metallic.png");
	m_material->setTex("MetallicMap", metallicTexture);

	auto roughnessTexture =  TextureMgr::shared()->getByPath("Texture/metalgrid3-ue/metalgrid3_roughness.png");
	m_material->setTex("RoughnessMap", roughnessTexture);


	auto normalMapTexture =  TextureMgr::shared()->getByPath("Texture/metalgrid3-ue/metalgrid3_normal-dx.png");
	m_material->setTex("NormalMap", normalMapTexture);
	m_node->setMaterial(m_material);

	m_shape = new PhysicsShape();
	m_shape->initBoxShape(vec3(blockSize, blockSize, blockSize));
	m_parent = nullptr;
	for(int i = 0; i < 6; i++)
	{
		m_bearPart[i] = nullptr;
	}
	initAttachments();
}

BlockPart::BlockPart(std::string itemName)
{
	initFromItemName(itemName);
}

void BlockPart::initAttachments()
{
	//forward backward
	addAttachment(new Attachment(vec3(0.0, 0.0, blockSize / 2.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0) ,this));
	addAttachment(new Attachment(vec3(0.0, 0.0, -blockSize / 2.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0) ,this));

	//right left
	addAttachment(new Attachment(vec3(blockSize / 2.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0) ,this));
	addAttachment(new Attachment(vec3(-blockSize / 2.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0) ,this));

	//up down
	addAttachment(new Attachment(vec3(0.0, blockSize / 2.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0) ,this));
	addAttachment(new Attachment(vec3(0.0, -blockSize / 2.0, 0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, 1.0) ,this));
}

Attachment * BlockPart::getAttachmentInfo(int index, vec3 & pos, vec3 & N, vec3 & up)
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

Attachment* BlockPart::getFirstAttachment()
{
	return m_attachment[0];
}

Attachment* BlockPart::getBottomAttachment()
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

Attachment* BlockPart::getAttachment(int index)
{
	return m_attachment[index];
}

int BlockPart::getAttachmentCount()
{
	return m_attachment.size();
}

GamePartType BlockPart::getType()
{
	return GamePartType::GAME_PART_BLOCK;
}
}
