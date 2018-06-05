#include "Renderer.h"
#include "GL/glew.h"
#include "../BackEnd/RenderBackEnd.h"
#include <algorithm>
#include "../Engine/Engine.h"
#include "../Scene/SceneMgr.h"
#include "../Mesh/Mesh.h"
#include "../3D/Sky.h"
#include "../2D/GUISystem.h"
#include "Technique/MaterialPool.h"
#include "../3D/ShadowMap/ShadowMap.h"

namespace tzw {
Renderer * Renderer::m_instance = nullptr;
Renderer::Renderer()
{
	m_enable3DRender = true;
	m_enableGUIRender = true;
	m_isNeedSortGUI = true;
	m_gbuffer = new RenderTarget();
	
	m_gbuffer->init(Engine::shared()->windowWidth(),Engine::shared()->windowHeight());
	m_offScreenBuffer = new RenderTarget();
	m_offScreenBuffer->init(Engine::shared()->windowWidth(),Engine::shared()->windowHeight(),1,false);
	m_dirLightProgram = MaterialPool::shared()->getMatFromTemplate("DirectLight");
	m_postEffect = new Material();
	m_postEffect->loadFromTemplate("postEffect");
	RenderBackEnd::shared()->setIsCheckGL(false);
	initQuad();
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
	switch(command.type())
	{
		case RenderCommand::RenderType::GUI:
			m_GUICommandList.push_back(command);
		break;
		case RenderCommand::RenderType::Common:
			if (command.getIsNeedTransparent())
			{
				m_transparentCommandList.push_back(command);
			}
			else
			{
				m_CommonCommand.push_back(command);
			}
			
		break;
		case RenderCommand::RenderType::Shadow:
			m_shadowCommandList.push_back(command);
		break;
		case RenderCommand::RenderType::Instanced:
		{
			m_CommonCommand.push_back(command);
		}
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
	Engine::shared()->setDrawCallCount(m_transparentCommandList.size() + m_CommonCommand.size() + m_GUICommandList.size());

	shadowPass();
	geometryPass();
	//prepareDeferred();
	m_offScreenBuffer->bindForWriting();
	m_gbuffer->bindForReadingGBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::AlphaBlend);
	RenderBackEnd::shared()->setBlendEquation(RenderFlag::BlendingEquation::Add);
	RenderBackEnd::shared()->setBlendFactor(RenderFlag::BlendingFactor::One,
		RenderFlag::BlendingFactor::One);
	RenderBackEnd::shared()->setDepthMaskWriteEnable(false);
	RenderBackEnd::shared()->setDepthTestEnable(false);
	directionalLightPass();
	//LightingPass();
	skyBoxPass();
	postEffectPass();
	if(m_enableGUIRender)
	{
		renderAllGUI();
	}
	clearCommands();
}

void Renderer::renderAllCommon()
{
	for(auto i = m_CommonCommand.begin();i!=m_CommonCommand.end();++i)
	{
		RenderCommand &command = (*i);
		renderCommon(command);
	}

}

void Renderer::renderAllShadow(int index)
{
	for (auto i = m_shadowCommandList.begin(); i != m_shadowCommandList.end(); ++i)
	{
		RenderCommand &command = (*i);
		renderShadow(command, index);
	}
}

void Renderer::renderAllTransparent()
{
	for(auto i = m_transparentCommandList.begin();i!=m_transparentCommandList.end();++i)
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

	GUISystem::shared()->tryRender();
	 

	//if (m_isNeedSortGUI)
	//{
	//	m_isNeedSortGUI = false;
	//	std::stable_sort(m_GUICommandList.begin(),m_GUICommandList.end(),GUICommandSort);
	//}
	//RenderBackEnd::shared()->disableFunction(RenderFlag::RenderFunction::DepthTest);
	//for(auto i =m_GUICommandList.begin();i!=m_GUICommandList.end();i++)
	//{
	//	RenderCommand &command = (*i);
	//	renderGUI(command);
	//}
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

void Renderer::renderShadow(RenderCommand &command,int index)
{
	//replace by the shadow shader, a little bit hack
	command.m_material->use(ShadowMap::shared()->getProgram());
	  
	applyRenderSetting(command.m_material);
	  
	auto cpyTransInfo = command.m_transInfo;
	  
	// one more little hack for light view & project matrix
	cpyTransInfo.m_viewMatrix = ShadowMap::shared()->getLightViewMatrix();
	cpyTransInfo.m_projectMatrix = ShadowMap::shared()->getLightProjectionMatrix(index);
	  
	applyTransform(ShadowMap::shared()->getProgram(), cpyTransInfo);

	auto viewMatrix = ShadowMap::shared()->getLightViewMatrix();
	auto lightWVP = ShadowMap::shared()->getLightProjectionMatrix(index) * viewMatrix * cpyTransInfo.m_worldMatrix;
	ShadowMap::shared()->getProgram()->setUniformMat4v("TU_lightWVP", lightWVP.data());
	RenderBackEnd::shared()->setDepthMaskWriteEnable(true);
	RenderBackEnd::shared()->setDepthTestEnable(true);
	if (command.type() == RenderCommand::RenderType::Instanced)
	{
		//renderPrimitveInstanced(command.m_mesh, command.m_material, command.m_primitiveType);
	}
	else
	{
		renderPrimitive(command.m_mesh, command.m_material, command.m_primitiveType, ShadowMap::shared()->getProgram());
	}
}

void Renderer::renderTransparent(RenderCommand & command)
{
	render(command);
}

void Renderer::clearCommands()
{
	m_CommonCommand.clear();
	m_GUICommandList.clear();
	m_transparentCommandList.clear();
	m_shadowCommandList.clear();
}

void Renderer::render(const RenderCommand &command)
{
	command.m_material->use();
	applyRenderSetting(command.m_material);
	applyTransform(command.m_material->getProgram(), command.m_transInfo);
	if (command.type() == RenderCommand::RenderType::Instanced)
	{
		renderPrimitveInstanced(command.m_mesh, command.m_material, command.m_primitiveType);
	}
	else
	{
		renderPrimitive(command.m_mesh, command.m_material, command.m_primitiveType);
	}
	
}

void Renderer::renderPrimitive(Mesh * mesh, Material * effect,RenderCommand::PrimitiveType primitiveType, ShaderProgram * extraProgram)
{
	auto program = effect->getProgram();
	  
	if (extraProgram)
	{
		program = extraProgram;
	}
	  
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
	  
	if (normalLocation > 0)
	{
		program->enableAttributeArray(normalLocation);
		program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
		  
	}
	  
	offset += sizeof(vec3);

	// Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
	int texcoordLocation = program->attributeLocation("a_texcoord");
	  
	if (texcoordLocation > 0)
	{
		program->enableAttributeArray(texcoordLocation);
		program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));
		
		  
	}
	
	offset += sizeof(vec2);
	int colorLocation = program->attributeLocation("a_color");
	  
	if (colorLocation > 0)
	{
		program->enableAttributeArray(colorLocation);
		program->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 4, sizeof(VertexData));
	}
	//  
	offset += sizeof(vec4);
	int bcLoaction = program->attributeLocation("a_bc");
	  
	if (bcLoaction > 0)
	{
		program->enableAttributeArray(bcLoaction);
		program->setAttributeBuffer(bcLoaction, GL_FLOAT, offset, 3, sizeof(VertexData));
	}
	offset += sizeof(vec3);

	int matLocation = program->attributeLocation("a_mat");
	  
	if (matLocation > 0)
	{
		program->enableAttributeArray(matLocation);
		program->setAttributeBuffer(matLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
	}

	mesh->getArrayBuf()->use();
	mesh->getIndexBuf()->use();
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
#define RAISE error = glGetError();printf("fuck error %d\n",error);
void Renderer::renderPrimitveInstanced(Mesh * mesh, Material * effect, RenderCommand::PrimitiveType primitiveType)
{
	glDisable(GL_CULL_FACE);
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
	if(normalLocation >= 0)
	{
		program->enableAttributeArray(normalLocation);
		program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
	}
	offset += sizeof(vec3);
	// Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
	int texcoordLocation = program->attributeLocation("a_texcoord");
	if(texcoordLocation >= 0)
	{
		program->enableAttributeArray(texcoordLocation);
		program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));
	}
	offset += sizeof(vec2);
	
	//int colorLocation = program->attributeLocation("a_color");
	//program->enableAttributeArray(colorLocation);
	//program->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 4, sizeof(VertexData));
	//offset += sizeof(vec4);

	//int bcLoaction = program->attributeLocation("a_bc");
	//program->enableAttributeArray(bcLoaction);
	//program->setAttributeBuffer(bcLoaction, GL_FLOAT, offset, 3, sizeof(VertexData));
	//offset += sizeof(vec3);

	//int matLocation = program->attributeLocation("a_mat");
	//program->enableAttributeArray(matLocation);
	//program->setAttributeBuffer(matLocation, GL_FLOAT, offset, 3, sizeof(VertexData));


	//RAISE
	mesh->getInstanceBuf()->use();
	int grassOffsetLocation = program->attributeLocation("a_instance_offset");
	program->enableAttributeArray(grassOffsetLocation);
	program->setAttributeBuffer(grassOffsetLocation, GL_FLOAT, 0, 4, 0);
	glVertexAttribDivisor(grassOffsetLocation, 1);

	switch (primitiveType)
	{
	case RenderCommand::PrimitiveType::TRIANGLES:
		RenderBackEnd::shared()->drawElementInstanced(RenderFlag::IndicesType::Triangles, mesh->getIndicesSize(), 0, mesh->getInstanceSize());
		break;
	case RenderCommand::PrimitiveType::Lines: break;
	case RenderCommand::PrimitiveType::TRIANGLE_STRIP: break;
	case RenderCommand::PrimitiveType::PATCHES: break;
	default: ;
	}
	glVertexAttribDivisor(grassOffsetLocation, 0);
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

void Renderer::geometryPass()
{
	m_gbuffer->bindForWriting();
	RenderBackEnd::shared()->setDepthMaskWriteEnable(true);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderBackEnd::shared()->selfCheck();
	if(Engine::shared()->getIsEnableOutLine())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		RenderBackEnd::shared()->selfCheck();
	}
	RenderBackEnd::shared()->setDepthTestEnable(true);
	RenderBackEnd::shared()->disableFunction(RenderFlag::RenderFunction::AlphaBlend);
	if(m_enable3DRender)
	{
		renderAllCommon();
		if (!m_transparentCommandList.empty())
		{
			RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::AlphaBlend);
			renderAllTransparent();	
			RenderBackEnd::shared()->disableFunction(RenderFlag::RenderFunction::AlphaBlend);
		}
	}
	if(Engine::shared()->getIsEnableOutLine())
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		RenderBackEnd::shared()->selfCheck();
	}
}

void Renderer::LightingPass()
{
	directionalLightPass();
}

void Renderer::shadowPass()
{
	
	ShadowMap::shared()->calculateProjectionMatrix();
	auto aabb = ShadowMap::shared()->getPotentialRange();
	std::vector<Drawable3D *> shadowNeedDrawList;
	g_GetCurrScene()->getRange(&shadowNeedDrawList, aabb);
	for(auto obj:shadowNeedDrawList)
	{
		obj->submitDrawCmd(RenderCommand::RenderType::Shadow);
	}
	ShadowMap::shared()->getProgram()->use();

	glDepthMask (true);
	RenderBackEnd::shared()->selfCheck();
	for (int i = 0 ; i < SHADOWMAP_CASCADE_NUM ; i++) 
	{
		auto shadowBuffer = ShadowMap::shared()->getFBO(i);
		shadowBuffer->BindForWriting();
		glClear(GL_DEPTH_BUFFER_BIT);
		RenderBackEnd::shared()->selfCheck();
		if (m_enable3DRender)
		{
			glDisable(GL_CULL_FACE);
			RenderBackEnd::shared()->selfCheck();
			renderAllShadow(i);
		}
	}


}

void Renderer::skyBoxPass()
{
	if(Sky::shared()->isEnable())
	{
		RenderBackEnd::shared()->setIsCullFace(true);
		auto mat = Sky::shared()->getMaterial();
		mat->setVar("TU_Depth", 0);
		auto dirLight = g_GetCurrScene()->getDirectionLight();
		mat->setVar("sun_pos",  dirLight->dir());
		mat->setVar("TU_winSize", Engine::shared()->winSize());
		mat->use();
		m_gbuffer->bindForReading();
		m_gbuffer->bindDepth(0);

		TransformationInfo info;
		Sky::shared()->setUpTransFormation(info);
		applyTransform(mat->getProgram(), info);
		Sky::shared()->prepare();
		renderPrimitive(Sky::shared()->getMesh(), mat, RenderCommand::PrimitiveType::TRIANGLES);
		RenderBackEnd::shared()->setIsCullFace(true);
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
		applyTransform(mat->getProgram(), info);
		renderPrimitive(skyBox->skyBoxMesh(), mat, RenderCommand::PrimitiveType::TRIANGLES);
	}
}

void Renderer::postEffectPass()
{
	auto currScene = g_GetCurrScene();
	m_offScreenBuffer->bindForReadingGBuffer();
	bindScreenForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto program = m_postEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniform2Float("TU_winSize", Engine::shared()->winSize());
	program->use();
	renderPrimitive(m_quad, m_postEffect, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::directionalLightPass()
{
	auto currScene = g_GetCurrScene();
	  
	m_dirLightProgram->use();
	  
	auto program = m_dirLightProgram->getProgram();
	  
	program->use();
	  
	program->setUniformInteger("TU_colorBuffer",0);
	  
	program->setUniformInteger("TU_posBuffer",1);
	  
	program->setUniformInteger("TU_normalBuffer",2);
	  
	program->setUniformInteger("TU_GBUFFER4",3);
	  
	program->setUniformInteger("TU_Depth", 4);


	program->setUniformInteger("TU_ShadowMap[0]", 5);
	ShadowMap::shared()->getFBO(0)->BindForReading(5);

	program->setUniformInteger("TU_ShadowMap[1]", 6);
	ShadowMap::shared()->getFBO(1)->BindForReading(6);

	program->setUniformInteger("TU_ShadowMap[2]", 7);
	ShadowMap::shared()->getFBO(2)->BindForReading(7);
	  

	program->setUniform2Float("TU_winSize", Engine::shared()->winSize());
	 
	
	for(int i = 0; i< SHADOWMAP_CASCADE_NUM; i++)
	{	
		char name[128] = { 0 };

		//Light View Projection Matrix
		auto lightVP = ShadowMap::shared()->getLightProjectionMatrix(i) * ShadowMap::shared()->getLightViewMatrix();
        sprintf_s(name, sizeof(name), "TU_LightVP[%d]", i);
		program->setUniformMat4v(name, lightVP.data());	


		//shadow Map cascade End distance
		sprintf_s(name, sizeof(name), "TU_ShadowMapEnd[%d]", i);
		program->setUniformFloat(name, ShadowMap::shared()->getCascadeEnd(i));
	}

	  
	auto cam = currScene->defaultCamera();
	  

	auto dirLight = currScene->getDirectionLight();
	  
	auto dirInViewSpace = (cam->getViewMatrix() * vec4(dirLight->dir(),0.0)).toVec3();
	  
	program->setUniform3Float("gDirectionalLight.direction",dirInViewSpace);
	  
	program->setUniform3Float("gDirectionalLight.color",dirLight->color());
	  
	program->setUniformFloat("gDirectionalLight.intensity", dirLight->intensity());
	  
	program->setUniformMat4v("TU_viewProjectInverted", cam->getViewProjectionMatrix().inverted().data());
	
	program->setUniformMat4v("TU_viewInverted", cam->getViewMatrix().inverted().data());

	auto ambient = currScene->getAmbient();
	  
	program->setUniform3Float("gAmbientLight.color",ambient->color());
	  
	program->setUniformFloat("gAmbientLight.intensity",ambient->intensity());
	  

	program->use();
	  
	renderPrimitive(m_quad, m_dirLightProgram, RenderCommand::PrimitiveType::TRIANGLES);
	  
}

void Renderer::applyRenderSetting(Material * mat)
{
	glDisable(GL_CULL_FACE);
	RenderBackEnd::shared()->selfCheck();
	//RenderBackEnd::shared()->setIsCullFace(true);
	//glCullFace(GL_NONE);
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

void Renderer::bindScreenForWriting()
{
	auto size = Engine::shared()->winSize();
	glViewport(0, 0, int(size.x), int(size.y));
	RenderBackEnd::shared()->selfCheck();
	RenderBackEnd::shared()->bindFrameBuffer(0);
}

	void Renderer::prepareDeferred()
{
	m_offScreenBuffer->bindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderBackEnd::shared()->selfCheck();
	RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::AlphaBlend);
	RenderBackEnd::shared()->setBlendEquation(RenderFlag::BlendingEquation::Add);
	RenderBackEnd::shared()->setBlendFactor(RenderFlag::BlendingFactor::One,
		RenderFlag::BlendingFactor::One);
	RenderBackEnd::shared()->setDepthMaskWriteEnable(false);
	RenderBackEnd::shared()->setDepthTestEnable(false);
	m_gbuffer->bindForReadingGBuffer();
}

} // namespace tzw

