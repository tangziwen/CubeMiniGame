#include "LaserPrimitive.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
#include "LinePrimitive.h"

namespace tzw {

LaserPrimitive::LaserPrimitive(vec3 begin, vec3 end, float width, bool isOriginInStart):m_isOriginInStart(isOriginInStart)
{

	m_begin = vec3(0, 0, 0);
	m_end = vec3(0, (end - begin).length(), 0);
	m_width = width;
	setIsAccpectOcTtree(false);
	initBuffer();
	init();
	if(isOriginInStart)
	{
		Node::setPos(begin);
	} else
	{
		Node::setPos(end);
	}
	
	Matrix44 mat;
	auto data = mat.data();
	vec3 up;
	if(isOriginInStart)
	{
		up = (end - begin).normalized();
	} else
	{
		up = (begin - end).normalized();
	}
	
	vec3 right = vec3::CrossProduct(vec3(0, 1, 0), up);
	vec3 front = vec3::CrossProduct(right, up);
	data[0] = right.x;
	data[1] = right.y;
	data[2] = right.z;

	data[4] = up.x;
	data[5] = up.y;
	data[6] = up.z;

	data[8] = front.x;
	data[9] = front.y;
	data[10] = front.z;
	Quaternion q;
	q.fromRotationMatrix(&mat);
	Node::setRotateQ(q);
}

void LaserPrimitive::submitDrawCmd(RenderFlag::RenderStageType stageType, RenderQueues * queues, int requirementArg)
{
	RenderCommand command(m_mesh,m_material, this, stageType);
    setUpTransFormation(command.m_transInfo);
	command.setRenderState(RenderFlag::RenderStage::TRANSPARENT);
	//command.setPrimitiveType(RenderCommand::PrimitiveType::Lines);
	 queues->addRenderCommand(command, requirementArg);
}

void LaserPrimitive::initBuffer()
{
	m_mesh = new Mesh();
	vec3 lB =m_begin - vec3(0.05, 0, 0);
	vec3 rB =m_begin + vec3(0.05, 0, 0);

	vec3 lE =m_end - vec3(0.05, 0, 0);
	vec3 rE =m_end + vec3(0.05, 0, 0);

	m_mesh->addVertex(VertexData(lB, vec2(0.0f, 0.0f)));
	m_mesh->addVertex(VertexData(rB,vec2(0.0f, 1.0f)));
	m_mesh->addVertex(VertexData(rE,vec2(1.0f, 1.0f)));
	m_mesh->addVertex(VertexData(lE,vec2(1.0f, 0.0f)));
	
	m_mesh->addIndex(0);
	m_mesh->addIndex(1);
	m_mesh->addIndex(2);
	
	m_mesh->addIndex(2);
	m_mesh->addIndex(3);
	m_mesh->addIndex(0);
	m_mesh->finish();
	m_localAABB.merge(m_mesh->getAabb());
	reCache();
	reCacheAABB();
}


void LaserPrimitive::init()
{
	m_material = Material::createFromTemplate("ModelWithYAxis");
	auto texture =  TextureMgr::shared()->getByPath("Texture/laser.png");
	m_material->setFactorDst(RenderFlag::BlendingFactor::One);
	m_material->setFactorSrc(RenderFlag::BlendingFactor::SrcAlpha);
	m_material->setTex("DiffuseMap", texture);
	m_material->setIsCullFace(false);
	setMaterial(m_material);
	setCamera(g_GetCurrScene()->defaultCamera());
}
	
} // namespace tzw
