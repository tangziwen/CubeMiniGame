#include "Renderer.h"
#include "GL/glew.h"
#include "../BackEnd/RenderBackEnd.h"
#include <algorithm>
#include "../Engine/Engine.h"
#include "../Scene/SceneMgr.h"
#include "../Mesh/Mesh.h"
#include "../3D/Sky.h"
namespace tzw {

Renderer * Renderer::m_instance = nullptr;
Renderer::Renderer()
{
	m_enable3DRender = true;
	m_enableGUIRender = true;
	m_isNeedSortGUI = true;
	m_gbuffer = new RenderTarget();
	m_gbuffer->init(Engine::shared()->windowWidth(),Engine::shared()->windowHeight());
	m_dirLightProgram = new Effect();
	m_dirLightProgram->load("DirectLight");
	initQuad();
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_CULL_FACE);
}

Renderer *Renderer::shared()
{
	if(!Renderer::m_instance)
	{
		Renderer::m_instance = new Renderer();
	}
	return Renderer::m_instance;
}

void Renderer::addRenderCommand(RenderCommand command)
{
	Engine::shared()->increaseDrawCallCount();
	switch(command.type())
	{
		case RenderCommand::RenderType::GUI:
			m_GUICommandList.push_back(command);
		break;
		case RenderCommand::RenderType::Common:
			m_CommonCommand.push_back(command);
		break;
		default:
		break;
	}
}

bool GUICommandSort(const RenderCommand &a,const RenderCommand &b)
{
	return a.Zorder() < b.Zorder();
}

void Renderer::renderAll()
{
	if(Engine::shared()->getIsEnableOutLine())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	
	RenderBackEnd::shared()->setDepthMaskWriteEnable(true);
	RenderBackEnd::shared()->setDepthTestEnable(true);
	m_gbuffer->bindForWriting();
	RenderBackEnd::shared()->disableFunction(RenderFlag::RenderFunction::AlphaBlend);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(m_enable3DRender)
	{
		renderAllCommon();
	}
	RenderBackEnd::shared()->bindFrameBuffer(0);
	if(Engine::shared()->getIsEnableOutLine())
	{
		glPolygonMode(GL_FRONT, GL_FILL);
	}
	LightingPass();
	skyBoxPass();
	

	if(m_enableGUIRender)
	{
		renderAllGUI();
	}

	clearCommands();
}

void Renderer::renderAllCommon()
{
	for(auto i = m_CommonCommand.begin();i!=m_CommonCommand.end();i++)
	{
		RenderCommand &command = (*i);
		renderCommon(command);
	}
}

void Renderer::renderAllGUI()
{
	RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::AlphaBlend);
	RenderBackEnd::shared()->setBlendFactor(RenderFlag::BlendingFactor::SrcAlpha,
											RenderFlag::BlendingFactor::OneMinusSrcAlpha);
	if (m_isNeedSortGUI)
	{
		m_isNeedSortGUI = false;
		std::stable_sort(m_GUICommandList.begin(),m_GUICommandList.end(),GUICommandSort);
	}
	RenderBackEnd::shared()->disableFunction(RenderFlag::RenderFunction::DepthTest);
	for(auto i =m_GUICommandList.begin();i!=m_GUICommandList.end();i++)
	{
		RenderCommand &command = (*i);
		renderGUI(command);
	}
	RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::DepthTest);
}

void Renderer::renderGUI(RenderCommand &command)
{
	render(command);
}

void Renderer::renderCommon(RenderCommand &command)
{
	//RenderBackEnd::shared()->setDepthTestMethod(command.depthTestPolicy().depthTest());
	render(command);
}

void Renderer::clearCommands()
{
	m_CommonCommand.clear();
	m_GUICommandList.clear();
}

void Renderer::render(const RenderCommand &command)
{
	command.m_material->use();
	applyTransform(command.m_material->getEffect()->getProgram(), command.m_transInfo);
	renderPrimitive(command.m_mesh, command.m_material->getEffect(), command.m_primitiveType);
}

void Renderer::renderPrimitive(Mesh * mesh, Effect * effect,RenderCommand::PrimitiveType primitiveType)
{
	auto program = effect->getProgram();
	program->use();
	mesh->getArrayBuf()->use();
	mesh->getIndexBuf()->use();
	// Offset for position
	unsigned int offset = 0;
	Engine::shared()->increaseVerticesIndicesCount(mesh->getVerticesSize(), mesh->getIndicesSize());
	// Tell OpenGL programmable pipeline how to locate vertex position data
	int vertexLocation = program->attributeLocation("a_position");
	program->enableAttributeArray(vertexLocation);
	program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
	offset += sizeof(vec3);

	int normalLocation = program->attributeLocation("a_normal");
	program->enableAttributeArray(normalLocation);
	program->setAttributeBuffer(normalLocation,GL_FLOAT,offset,3,sizeof(VertexData));
	offset += sizeof(vec3);

	// Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
	int texcoordLocation = program->attributeLocation("a_texcoord");
	program->enableAttributeArray(texcoordLocation);
	program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));
	offset += sizeof(vec2);

	int colorLocation = program->attributeLocation("a_color");
	program->enableAttributeArray(colorLocation);
	program->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 4, sizeof(VertexData));
	offset += sizeof(vec4);

	int bcLoaction = program->attributeLocation("a_bc");
	program->enableAttributeArray(bcLoaction);
	program->setAttributeBuffer(bcLoaction, GL_FLOAT, offset, 3, sizeof(VertexData));

	switch(primitiveType)
	{
		case RenderCommand::PrimitiveType::Lines:
			RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::Lines,mesh->getIndicesSize(),0);
		break;
		case RenderCommand::PrimitiveType::TRIANGLES:
			RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::Triangles,mesh->getIndicesSize(),0);
		break;
		case RenderCommand::PrimitiveType::TRIANGLE_STRIP:
			RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::TriangleStrip,mesh->getIndicesSize(),0);
		break;
		case RenderCommand::PrimitiveType::PATCHES:
			RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::Patches,mesh->getIndicesSize(),0);
		break;
	}
}

///
/// \brief 获取是否禁用3D渲染
/// \return
///
bool Renderer::enable3DRender() const
{
	return m_enable3DRender;
}
///
/// \brief 决定是否禁用3D渲染
/// \param enable3DRender
///
void Renderer::setEnable3DRender(bool enable3DRender)
{
	m_enable3DRender = enable3DRender;
}

///
/// \brief 获取是否渲染GUI
/// \return
///
bool Renderer::enableGUIRender() const
{
	return m_enableGUIRender;
}

///
/// \brief 决定是否禁用GUI渲染
/// \param enableGUIRender
///
void Renderer::setEnableGUIRender(bool enableGUIRender)
{
	m_enableGUIRender = enableGUIRender;
}

void Renderer::notifySortGui()
{
	m_isNeedSortGUI = true;
}

void Renderer::initQuad()
{
	m_quad = new Mesh();
	m_quad->addVertex(VertexData(vec3(-1.0,-1.0,0.0),vec2(0.0,0.0)));
	m_quad->addVertex(VertexData(vec3(1.0,-1.0,0.0),vec2(1.0,0.0)));
	m_quad->addVertex(VertexData(vec3(1.0,1.0,0.0),vec2(1.0,1.0)));
	m_quad->addVertex(VertexData(vec3(-1.0,1.0,0.0),vec2(0.0,1.0)));
	unsigned short indics[] = {0,1,2,0,2,3};
	m_quad->addIndices(indics,6);
	m_quad->finish(true);
}

void Renderer::LightingPass()
{
	RenderBackEnd::shared()->bindFrameBuffer(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BITS);
	RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::AlphaBlend);
	RenderBackEnd::shared()->setBlendEquation(RenderFlag::BlendingEquation::Add);
	RenderBackEnd::shared()->setBlendFactor(RenderFlag::BlendingFactor::One,
											RenderFlag::BlendingFactor::One);
	RenderBackEnd::shared()->setDepthMaskWriteEnable(false);
	RenderBackEnd::shared()->setDepthTestEnable(false);

	m_gbuffer->bindForReadingGBuffer();
	
	
	directionalLightPass();
}

void Renderer::skyBoxPass()
{
	if(Sky::shared()->isEnable())
	{
		auto mat = Sky::shared()->getMaterial();
		mat->use();
		m_gbuffer->bindForReading();
		m_gbuffer->bindDepth(0);
		mat->setVar("TU_Depth",0);
		mat->setVar("TU_winSize", Engine::shared()->winSize());
		TransformationInfo info;
		Sky::shared()->setUpTransFormation(info);
		applyTransform(mat->getEffect()->getProgram(), info);
		Sky::shared()->prepare();
		renderPrimitive(Sky::shared()->getMesh(), mat->getEffect(), RenderCommand::PrimitiveType::TRIANGLES);
	}else
	{
		auto skyBox = g_GetCurrScene()->getSkyBox();
		if(!skyBox) return;
		skyBox->prepare();
		auto mat = skyBox->getMaterial();
		mat->use();
		m_gbuffer->bindForReading();
		m_gbuffer->bindDepth(1);
		mat->setVar("TU_Depth",1);
		mat->setVar("TU_winSize", Engine::shared()->winSize());
		TransformationInfo info;
		skyBox->setUpTransFormation(info);
		applyTransform(mat->getEffect()->getProgram(), info);
		renderPrimitive(skyBox->skyBoxMesh(), mat->getEffect(), RenderCommand::PrimitiveType::TRIANGLES);
	}
}

void Renderer::directionalLightPass()
{

	auto currScene = g_GetCurrScene();
	auto program = m_dirLightProgram->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniformInteger("TU_posBuffer",1);
	program->setUniformInteger("TU_normalBuffer",2);
	program->setUniformInteger("TU_GBUFFER4",3);
	program->setUniformInteger("TU_Depth", 4);
	program->setUniform2Float("TU_winSize", Engine::shared()->winSize());
	auto cam = currScene->defaultCamera();

	auto dirLight = currScene->getDirectionLight();
	auto dirInViewSpace = (cam->getViewMatrix() * vec4(dirLight->dir(),0.0)).toVec3();
	program->setUniform3Float("gDirectionalLight.direction",dirInViewSpace);
	program->setUniform3Float("gDirectionalLight.color",dirLight->color());

	auto ambient = currScene->getAmbient();
	program->setUniform3Float("gAmbientLight.color",ambient->color());
	program->setUniformFloat("gAmbientLight.intensity",ambient->intensity());

	program->use();
	renderPrimitive(m_quad, m_dirLightProgram, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::applyTransform(ShaderProgram *program, const TransformationInfo &info)
{
	program->use();
	auto p = info.m_projectMatrix;
	auto v = info.m_viewMatrix;
	auto m = info.m_worldMatrix;
	auto vp = p * v;

	program->setUniformMat4v("TU_mvpMatrix", (vp* m).data());
	program->setUniformMat4v("TU_vpMatrix", vp.data());
	program->setUniformMat4v("TU_mvMatrix", (v * m).data());
	program->setUniformMat4v("TU_vMatrix", v.data());

	program->setUniformMat4v("TU_mMatrix", m.data());
	program->setUniformMat4v("TU_normalMatrix", (v * m).inverted().transpose().data());
}




} // namespace tzw

