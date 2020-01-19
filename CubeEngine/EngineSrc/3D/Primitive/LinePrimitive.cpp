#include "LinePrimitive.h"
#include "../../Rendering/Renderer.h"
#include "../../Scene/SceneMgr.h"
namespace tzw {

LinePrimitive::LinePrimitive(vec3 begin, vec3 end)
{
	append(begin, end);
	
	init();
	initBuffer();
}

LinePrimitive::LinePrimitive():m_mesh(nullptr)
{
	init();
}

void LinePrimitive::submitDrawCmd(RenderCommand::RenderType passType)
{
	RenderCommand command(m_mesh,m_material,RenderCommand::RenderType::Common);
    setUpTransFormation(command.m_transInfo);
	command.setPrimitiveType(RenderCommand::PrimitiveType::Lines);
    Renderer::shared()->addRenderCommand(command);
}

void LinePrimitive::initBuffer()
{
	if(!m_mesh)
	{
		m_mesh = new Mesh();
	}
	int i = 0;
	for(auto s : m_segList)
	{
		m_mesh->addVertex(VertexData(s.begin,s.color));
		m_mesh->addVertex(VertexData(s.end,s.color));
		m_mesh->addIndex(i);
		m_mesh->addIndex(i + 1);
		i += 2;
	}
	//it is Enough room for new Vertex
	if(m_mesh->getIndexBuf()->getAmount() >= m_mesh->getIndicesSize())
	{
		m_mesh->reSubmit();
	}else
	{
		m_mesh->submit(RenderFlag::BufferStorageType::DYNAMIC_DRAW);
	}
	reCache();
}

void LinePrimitive::setUpTransFormation(TransformationInfo & info)
{
	auto currCam = g_GetCurrScene()->defaultCamera();
	
	info.m_projectMatrix = currCam->projection();
    info.m_viewMatrix = currCam->getViewMatrix();
    info.m_worldMatrix = getTransform();
	info.m_worldMatrix.stripScale();
}

void LinePrimitive::append(vec3 begin, vec3 end, vec3 color)
{
	LineInfo info;
	info.begin = begin;
	info.color = color;
	info.end = end;
	m_segList.push_back(info);
}

void LinePrimitive::clear()
{
	if(m_mesh)
	{
		m_mesh->clearIndices();
		m_mesh->clearVertices();
	}

	m_segList.clear();
}

int LinePrimitive::getLineCount()
{
	return m_segList.size();
}

void LinePrimitive::init()
{
	m_material = Material::createFromTemplate("Color");
	m_material->setIsDepthTestEnable(false);
	m_material->setRenderStage(RenderFlag::RenderStage::TRANSPARENT);
	setCamera(g_GetCurrScene()->defaultCamera());
}
} // namespace tzw
