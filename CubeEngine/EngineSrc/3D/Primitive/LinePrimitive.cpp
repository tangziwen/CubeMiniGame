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

LinePrimitive::LinePrimitive()
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
	m_mesh = new Mesh();
	int i = 0;
	for(auto s : m_segList)
	{
		m_mesh->addVertex(VertexData(s.begin));
		m_mesh->addVertex(VertexData(s.end));
		m_mesh->addIndex(i);
		m_mesh->addIndex(i + 1);
		i += 2;
	}
	m_mesh->finish();
	reCache();
}

void LinePrimitive::setUpTransFormation(TransformationInfo & info)
{
	info.m_projectMatrix = camera()->projection();
    info.m_viewMatrix = camera()->getViewMatrix();
    info.m_worldMatrix = getTransform();
	info.m_worldMatrix.stripScale();
}

void LinePrimitive::append(vec3 begin, vec3 end)
{
	LineInfo info;
	info.begin = begin;
	info.end = end;
	m_segList.push_back(info);
}

void LinePrimitive::init()
{
	m_material = Material::createFromTemplate("Color");
	setCamera(g_GetCurrScene()->defaultCamera());
}
} // namespace tzw
