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
#include <random>
#include "Shader/ShaderMgr.h"
#include "3D/Thumbnail.h"
#include "3D/Vegetation/Tree.h"

namespace tzw {
Renderer * Renderer::m_instance = nullptr;
Renderer::Renderer(): m_quad(nullptr), m_dirLightProgram(nullptr), m_postEffect(nullptr), m_blurVEffect(nullptr),
					m_blurHEffect(nullptr), m_ssaoCompositeEffect(nullptr), m_gbuffer(nullptr),
					m_offScreenBuffer(nullptr), m_ssaoBuffer1(nullptr), m_ssaoBuffer2(nullptr),
					m_skyEnable(true), m_fogEnable(true), m_ssaoEnable(true), m_bloomEnable(true),
					m_hdrEnable(true), m_aaEnable(true)
{
	m_enable3DRender = true;
	m_enableGUIRender = true;
	m_isNeedSortGUI = true;
}

static FrameBuffer * m_autoExposure0;
static FrameBuffer * m_autoExposure1;
static FrameBuffer * m_autoExposure2;
static FrameBuffer * m_autoExposure3;
static FrameBuffer * m_autoExposure4;


static FrameBuffer * m_bloomBuffer_half1;
static FrameBuffer * m_bloomBuffer_half2;
static FrameBuffer * m_bloomBuffer_quad1;
static FrameBuffer * m_bloomBuffer_quad2;
static FrameBuffer * m_bloomBuffer_octave1;
static FrameBuffer * m_bloomBuffer_octave2;

static FrameBuffer * m_bloomResultBuffer;

static FrameBuffer * m_ssaoResultBuffer;
Renderer *Renderer::shared()
{
	if(!Renderer::m_instance)
	{
		Renderer::m_instance = new Renderer();
	}
	return Renderer::m_instance;
}

void Renderer::addRenderCommand(RenderCommand& command)
{
	switch(command.type())
	{
		case RenderCommand::RenderType::GUI:
			m_GUICommandList.push_back(command);
		break;
		case RenderCommand::RenderType::Common:
			switch (command.getRenderState())
			{
			case RenderFlag::RenderStage::COMMON: m_CommonCommand.push_back(command);break;
			case RenderFlag::RenderStage::TRANSPARENT: m_transparentCommandList.push_back(command);break;
			case RenderFlag::RenderStage::AFTER_DEPTH_CLEAR: m_clearDepthCommandList.push_back(command);break;
			default: ;
			}
		break;
		case RenderCommand::RenderType::Shadow:
			m_shadowCommandList.push_back(command);
		break;
		default:
		break;
	}
}

bool GUICommandSort(const RenderCommand &a,const RenderCommand &b)
{
	return a.Zorder() < b.Zorder();
}
std::vector<FrameBuffer *> autoExposureList;
void Renderer::renderAll()
{
	handleThumbNail();
	Engine::shared()->setDrawCallCount(int(m_transparentCommandList.size() + m_CommonCommand.size() + m_GUICommandList.size()));
	if(m_enable3DRender)
	{
		shadowPass();
		geometryPass();
		LightingPass();

		skyBoxPass();// same with lighting pass
		FogPass();// same with lighting pass
		if(m_ssaoEnable)
		{
			SSAOPass();
			for(int i = 0; i < 5; i++) 
			{
				SSAOBlurVPass();
				SSAOBlurHPass();
			}
			SSAOBlurCompossitPass();
		}else
		{
			copyToFrame(m_offScreenBuffer, m_ssaoResultBuffer, m_copyEffect);
			
		}
		//get the average luminance
		autoExposurePass();
		if(m_bloomEnable)
		{
			BloomBrightPass();
			//copy to multiple bloom
			copyToFrame(m_bloomBuffer1, m_bloomBuffer_half1, m_copyEffect);
			copyToFrame(m_bloomBuffer1, m_bloomBuffer_quad1, m_copyEffect);
			copyToFrame(m_bloomBuffer1, m_bloomBuffer_octave1, m_copyEffect);

			//ping pong
			for(int i = 0; i<3; i++) 
			{
			copyToFrame(m_bloomBuffer1, m_bloomBuffer2, m_BloomBlurVEffect);
			copyToFrame(m_bloomBuffer2, m_bloomBuffer1, m_BloomBlurHEffect);
			}
			for(int i = 0; i<3; i++) 
			{
			copyToFrame(m_bloomBuffer_half1, m_bloomBuffer_half2, m_BloomBlurVEffect);
			copyToFrame(m_bloomBuffer_half2, m_bloomBuffer_half1, m_BloomBlurHEffect);
	        }
			for(int i = 0; i<3; i++) 
			{
			copyToFrame(m_bloomBuffer_quad1, m_bloomBuffer_quad2, m_BloomBlurVEffect);
			copyToFrame(m_bloomBuffer_quad2, m_bloomBuffer_quad1, m_BloomBlurHEffect);
	        }
			for(int i = 0; i<3; i++) 
			{
			copyToFrame(m_bloomBuffer_octave1, m_bloomBuffer_octave2, m_BloomBlurVEffect);
			copyToFrame(m_bloomBuffer_octave2, m_bloomBuffer_octave1, m_BloomBlurHEffect);
	        }
			BloomCompossitPass();
		}else
		{
			copyToFrame(m_ssaoResultBuffer, m_bloomResultBuffer, m_copyEffect);
		}
		toneMappingPass();
		if(m_aaEnable)
		{
			AAPass();
		}

	}
	
	m_gbuffer->bindForReading();
	bindScreenForWriting();
	//copy depth buffer to default buffer for transparent object depth testing
	auto screenSize = Engine::shared()->winSize();
	auto gbufferSize = m_gbuffer->getFrameSize();
	glBlitFramebuffer(0, 0, gbufferSize.x, gbufferSize.y, 0, 0, screenSize.x, screenSize.y,
	                  GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	if (!m_transparentCommandList.empty())
	{
		RenderBackEnd::shared()->setDepthTestEnable(true);
		RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::AlphaBlend);
		RenderBackEnd::shared()->setBlendEquation(RenderFlag::BlendingEquation::Add);

		renderAllTransparent();	
		RenderBackEnd::shared()->disableFunction(RenderFlag::RenderFunction::AlphaBlend);
	}
	if(!m_clearDepthCommandList.empty())
	{
		RenderBackEnd::shared()->setDepthMaskWriteEnable(true);
		glClear(GL_DEPTH_BUFFER_BIT);
		RenderBackEnd::shared()->selfCheck();
		renderAllClearDepthTransparent();	
	}
	if(m_enableGUIRender)
	{
		RenderBackEnd::shared()->setDepthTestEnable(false);
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


void Renderer::renderAllClearDepthTransparent()
{
	for(auto i = m_clearDepthCommandList.begin();i!=m_clearDepthCommandList.end();++i)
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
	
	RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::DepthTest);
	for(auto iter:m_GUICommandList)
	{
		renderGUI(iter);
	}
	GUISystem::shared()->renderIMGUI();
	 
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

	if (command.batchType() == RenderCommand::RenderBatchType::Instanced)
	{
		auto program = ShadowMap::shared()->getInstancedProgram();
		//replace by the shadow shader, a little bit hack
		command.m_material->use(program);
		  
		applyRenderSetting(command.m_material);
		  
		auto cpyTransInfo = command.m_transInfo;
		  
		// one more little hack for light view & project matrix
		cpyTransInfo.m_viewMatrix = ShadowMap::shared()->getLightViewMatrix();
		cpyTransInfo.m_projectMatrix = ShadowMap::shared()->getLightProjectionMatrix(index);
		  
		applyTransform(program, cpyTransInfo);

		auto viewMatrix = ShadowMap::shared()->getLightViewMatrix();
		auto lightWVP = ShadowMap::shared()->getLightProjectionMatrix(index) * viewMatrix * cpyTransInfo.m_worldMatrix;
		program->setUniformMat4v("TU_lightWVP", lightWVP.data());
		RenderBackEnd::shared()->setDepthMaskWriteEnable(true);
		RenderBackEnd::shared()->setDepthTestEnable(true);
		renderPrimitveInstanced(command.m_mesh, command.m_material, command.m_primitiveType, program);
	}
	else
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
		renderPrimitive(command.m_mesh, command.m_material, command.m_primitiveType, ShadowMap::shared()->getProgram());
	}
}

void Renderer::init()
{
	initBuffer();

	//setIBL("Texture/IBL/desert_irradiance.dds", "Texture/IBL/desert_radiance.dds", false);
	setIBL("Texture/IBL/autumn_irradiance_latlong.dds", "Texture/IBL/autumn_radiance_latlong.dds", false);

	//setIBL("Texture/IBL/autumn_irradiance.dds", "Texture/IBL/autumn_radiance.dds", true);
	initMaterials();
	RenderBackEnd::shared()->setIsCheckGL(false);
	initQuad();
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	ssaoSetup();
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
	m_clearDepthCommandList.clear();
}

void Renderer::render(const RenderCommand &command)
{
	command.m_material->use();
	applyRenderSetting(command.m_material);
	applyTransform(command.m_material->getProgram(), command.m_transInfo);
	if (command.batchType() == RenderCommand::RenderBatchType::Instanced)
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
	  
	Engine::shared()->increaseVerticesIndicesCount(int(mesh->getVerticesSize()), int(mesh->getIndicesSize()));
	  
	// Tell OpenGL programmable pipeline how to locate vertex position data

	setVertexAttribute(program);
	switch(primitiveType)
	{
		case RenderCommand::PrimitiveType::Lines:
			RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::Lines, (unsigned)mesh->getIndicesSize(),0);
		break;
		case RenderCommand::PrimitiveType::TRIANGLES:
			RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::Triangles, (unsigned)mesh->getIndicesSize(),0);
		break;
		case RenderCommand::PrimitiveType::TRIANGLE_STRIP:
			RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::TriangleStrip, (unsigned)mesh->getIndicesSize(),0);
		break;
		case RenderCommand::PrimitiveType::PATCHES:
			RenderBackEnd::shared()->drawElement(RenderFlag::IndicesType::Patches, (unsigned)mesh->getIndicesSize(),0);
		break;
	}
}
#define RAISE error = glGetError();tlogError("raise error %d\n",error);
void Renderer::renderPrimitveInstanced(Mesh * mesh, Material * effect, RenderCommand::PrimitiveType primitiveType, ShaderProgram * extraProgram)
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
	Engine::shared()->increaseVerticesIndicesCount(mesh->getVerticesSize(), mesh->getIndicesSize());
	// Tell OpenGL programmable pipeline how to locate vertex position data
	setVertexAttribute(program);
	//RAISE
	mesh->getInstanceBuf()->use();
	int grassOffsetLocation = program->attributeLocation("a_instance_offset");
	program->enableAttributeArray(grassOffsetLocation);
	program->setAttributeBuffer(grassOffsetLocation, GL_FLOAT, 0, 4, sizeof(InstanceData));
	glVertexAttribDivisor(grassOffsetLocation, 1);
	
	int extraInstanceOffsetLocation = program->attributeLocation("a_instance_offset2");
	if(extraInstanceOffsetLocation > 0)
	{
		program->enableAttributeArray(extraInstanceOffsetLocation);
		program->setAttributeBuffer(extraInstanceOffsetLocation, GL_FLOAT, offsetof(InstanceData, extraInfo.x), 4, sizeof(InstanceData));
		glVertexAttribDivisor(extraInstanceOffsetLocation, 1);
	}

	int rotateInstanceOffset = program->attributeLocation("a_instance_offset3");
	if(rotateInstanceOffset > 0)
	{
		program->enableAttributeArray(rotateInstanceOffset);
		program->setAttributeBuffer(rotateInstanceOffset, GL_FLOAT, offsetof(InstanceData, rotateInfo.x), 4, sizeof(InstanceData));
		glVertexAttribDivisor(rotateInstanceOffset, 1);
	}
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
	if(extraInstanceOffsetLocation > 0)
	{
		glVertexAttribDivisor(extraInstanceOffsetLocation, 0);
	}
	if(extraInstanceOffsetLocation > 0)
	{
		glVertexAttribDivisor(extraInstanceOffsetLocation, 0);
	}
	if(rotateInstanceOffset > 0)
	{
		glVertexAttribDivisor(rotateInstanceOffset, 0);
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

void Renderer::initMaterials()
{
	m_dirLightProgram = MaterialPool::shared()->getMatFromTemplate("DirectLight");

	m_postEffect = new Material();
	m_postEffect->loadFromTemplate("SSAO");
	MaterialPool::shared()->addMaterial("SSAO", m_postEffect);

	m_blurVEffect = new Material();
	m_blurVEffect->loadFromTemplate("blurV");
	MaterialPool::shared()->addMaterial("blurV", m_blurVEffect);

	m_blurHEffect = new Material();
	m_blurHEffect->loadFromTemplate("blurH");
	MaterialPool::shared()->addMaterial("blurH", m_blurHEffect);

	m_ssaoCompositeEffect = new Material();
	m_ssaoCompositeEffect->loadFromTemplate("SSAOComposite");
	MaterialPool::shared()->addMaterial("SSAOComposite", m_ssaoCompositeEffect);



	m_BloomBlurVEffect = new Material();
	m_BloomBlurVEffect->loadFromTemplate("BloomBlurV");
	MaterialPool::shared()->addMaterial("BloomBlurV", m_BloomBlurVEffect);

	m_BloomBlurHEffect = new Material();
	m_BloomBlurHEffect->loadFromTemplate("BloomBlurH");
	MaterialPool::shared()->addMaterial("BloomBlurH", m_BloomBlurHEffect);

	m_bloomBrightPassEffect = new Material();
	m_bloomBrightPassEffect->loadFromTemplate("BloomBrightPass");
	MaterialPool::shared()->addMaterial("BloomBrightPass", m_bloomBrightPassEffect);

	

	m_BloomCompositePassEffect = new Material();
	m_BloomCompositePassEffect->loadFromTemplate("BloomCompositePass");
	MaterialPool::shared()->addMaterial("BloomCompositePass", m_BloomCompositePassEffect);


	m_autoExposurePassEffect = new Material();
	m_autoExposurePassEffect->loadFromTemplate("AutoExposurePass");
	MaterialPool::shared()->addMaterial("AutoExposurePass", m_autoExposurePassEffect);


	m_FXAAEffect = new Material();
	m_FXAAEffect->loadFromTemplate("FXAA");
	MaterialPool::shared()->addMaterial("FXAA", m_FXAAEffect);

	m_fogEffect = new Material();
	m_fogEffect->loadFromTemplate("GlobalFog");
	MaterialPool::shared()->addMaterial("GlobalFog", m_fogEffect);

	m_copyEffect = new Material();
	m_copyEffect->loadFromTemplate("CopyToFrame");
	MaterialPool::shared()->addMaterial("CopyToFrame", m_copyEffect);

	m_ToneMappingPassEffect = new Material();
	m_ToneMappingPassEffect->loadFromTemplate("ToneMappingPass");
	MaterialPool::shared()->addMaterial("ToneMappingPass", m_ToneMappingPassEffect);
}





void Renderer::initBuffer()
{
	float w = Engine::shared()->windowWidth();
	float h = Engine::shared()->windowHeight();
	m_gbuffer = new FrameBuffer();
	m_gbuffer->init(w, h,4,true,true);
	m_gbuffer->setIsLinearFilter(false);
	m_gbuffer->gen();

	m_offScreenBuffer = new FrameBuffer();
	m_offScreenBuffer->init(w, h,1,false, true);
	m_offScreenBuffer->gen();

	m_offScreenBuffer2 = new FrameBuffer();
	m_offScreenBuffer2->init(w, h,1,false, true);
	m_offScreenBuffer2->gen();

	m_bloomResultBuffer = new FrameBuffer();
	m_bloomResultBuffer->init(w, h,1,false, true);
	m_bloomResultBuffer->gen();


	m_ssaoResultBuffer = new FrameBuffer();
	m_ssaoResultBuffer->init(w, h,1,false, true);
	m_ssaoResultBuffer->gen();

	
	m_ssaoBuffer1 = new FrameBuffer();
	m_ssaoBuffer1->init(w * 0.5, h * 0.5, 1, false, true);
	m_ssaoBuffer1->gen();

	m_ssaoBuffer2 = new FrameBuffer();
	m_ssaoBuffer2->init(w * 0.5, h * 0.5, 1, false,true);
	m_ssaoBuffer2->gen();

	m_bloomBuffer1 = new FrameBuffer();
	m_bloomBuffer1->init(w * 0.5, h  * 0.5, 1, false,true);
	m_bloomBuffer1->gen();

	m_bloomBuffer2 = new FrameBuffer();
	m_bloomBuffer2->init(w * 0.5, h * 0.5, 1, false,true);
	m_bloomBuffer2->gen();


	m_bloomBuffer_half1 = new FrameBuffer();
	m_bloomBuffer_half1->init(w / 4.0f, h / 4.0f, 1, false,true);
	m_bloomBuffer_half1->gen();

	m_bloomBuffer_half2 = new FrameBuffer();
	m_bloomBuffer_half2->init(w / 4.0f, h / 4.0f, 1, false,true);
	m_bloomBuffer_half2->gen();


	m_bloomBuffer_quad1 = new FrameBuffer();
	m_bloomBuffer_quad1->init(w / 8.0f, h / 8.0f, 1, false,true);
	m_bloomBuffer_quad1->gen();


	m_bloomBuffer_quad2 = new FrameBuffer();
	m_bloomBuffer_quad2->init(w / 8.0f, h / 8.0f, 1, false,true);
	m_bloomBuffer_quad2->gen();


	m_bloomBuffer_octave1 = new FrameBuffer();
	m_bloomBuffer_octave1->init(w / 16.0f, h / 16.0f, 1, false,true);
	m_bloomBuffer_octave1->gen();

	m_bloomBuffer_octave2 = new FrameBuffer();
	m_bloomBuffer_octave2->init(w / 16.0f, h / 16.0f, 1, false,true);
	m_bloomBuffer_octave2->gen();

	int sizeArray[] = {512, 256, 128, 64, 32, 16, 8, 4, 2, 1};
	for(auto i = 0; i < sizeof(sizeArray) / sizeof(int); i ++) 
	{
		auto fb = new FrameBuffer();
		fb->init(sizeArray[i], sizeArray[i], 1, false,true);
		fb->gen();
		autoExposureList.push_back(fb);
	}
}

void Renderer::onChangeScreenSize(int newW, int newH)
{
	initBuffer();
	glScissor(0, 0, newW, newH);
}

void Renderer::updateThumbNail(ThumbNail* thumb)
{
	m_thumbNailList.push_back(thumb);
}

void Renderer::setVertexAttribute(ShaderProgram* program)
{
	int vertexLocation = program->attributeLocation("a_position");
	  
	program->enableAttributeArray(vertexLocation);
	  
	program->setAttributeBuffer(vertexLocation, GL_FLOAT, offsetof(VertexData, m_pos), 3, sizeof(VertexData));
	int normalLocation = program->attributeLocation("a_normal");
	  
	if (normalLocation > 0)
	{
		program->enableAttributeArray(normalLocation);
		program->setAttributeBuffer(normalLocation, GL_FLOAT, offsetof(VertexData, m_normal), 3, sizeof(VertexData));
		  
	}
	// Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
	int texcoordLocation = program->attributeLocation("a_texcoord");
	  
	if (texcoordLocation > 0)
	{
		program->enableAttributeArray(texcoordLocation);
		program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offsetof(VertexData, m_texCoord), 2, sizeof(VertexData));
	}
	int colorLocation = program->attributeLocation("a_color");
	  
	if (colorLocation > 0)
	{
		program->enableAttributeArray(colorLocation);
		program->setAttributeBuffer(colorLocation, GL_FLOAT, offsetof(VertexData, m_color), 4, sizeof(VertexData));
	}
	int bcLoaction = program->attributeLocation("a_bc");
	  
	if (bcLoaction > 0)
	{
		program->enableAttributeArray(bcLoaction);
		program->setAttributeBuffer(bcLoaction, GL_FLOAT, offsetof(VertexData, m_barycentric), 3, sizeof(VertexData));
	}

	int matLocation = program->attributeLocation("a_mat");
	if (matLocation > 0)
	{
		program->enableAttributeArray(matLocation);
		program->setAttributeBufferInt(matLocation, GL_BYTE, offsetof(VertexData, m_matIndex), 3, sizeof(VertexData));
	}
	int matBlendLocation = program->attributeLocation("a_matBlend");
	if (matBlendLocation > 0)
	{
		program->enableAttributeArray(matBlendLocation);
		program->setAttributeBuffer(matBlendLocation, GL_FLOAT, offsetof(VertexData, m_matBlendFactor), 3, sizeof(VertexData));
	}
	
	int tangentLocation = program->attributeLocation("a_tangent");
	if (tangentLocation > 0)
	{
		program->enableAttributeArray(tangentLocation);
		program->setAttributeBuffer(tangentLocation, GL_FLOAT, offsetof(VertexData, m_tangent), 3, sizeof(VertexData));
	}
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
	}
	if(Engine::shared()->getIsEnableOutLine())
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		RenderBackEnd::shared()->selfCheck();
	}
}

void Renderer::LightingPass()
{
	m_offScreenBuffer->bindForWriting();
	m_gbuffer->bindForReadingGBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	directionalLightPass();
}

void Renderer::shadowPass()
{
	if(!m_shadowEnable)
	{
		for (int i = 0 ; i < SHADOWMAP_CASCADE_NUM ; i++) 
		{
			m_shadowCommandList.clear();
			auto shadowBuffer = ShadowMap::shared()->getFBO(i);
			shadowBuffer->BindForWriting();
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		return;
	}

	ShadowMap::shared()->calculateProjectionMatrix();

	ShadowMap::shared()->getProgram()->use();
	glDepthMask (true);
	for (int i = 0 ; i < SHADOWMAP_CASCADE_NUM ; i++) 
	{
		m_shadowCommandList.clear();
		auto aabb = ShadowMap::shared()->getPotentialRange(i);
		std::vector<Drawable3D *> shadowNeedDrawList;
		g_GetCurrScene()->getRange(&shadowNeedDrawList, aabb);
		for(auto obj:shadowNeedDrawList)
		{
			obj->submitDrawCmd(RenderCommand::RenderType::Shadow);
		}
		Tree::shared()->submitShadowDraw();
		auto shadowBuffer = ShadowMap::shared()->getFBO(i);
		shadowBuffer->BindForWriting();
		glClear(GL_DEPTH_BUFFER_BIT);
		if (m_enable3DRender)
		{
			//glDisable(GL_CULL_FACE);
			renderAllShadow(i);
		}
	}


}

void Renderer::skyBoxPass()
{
	m_gbuffer->bindForReading();
	m_offScreenBuffer->bindForWriting();
	if(m_skyEnable)
	{
		if(Sky::shared()->isEnable())
		{
			RenderBackEnd::shared()->setIsCullFace(false);
			auto mat = Sky::shared()->getMaterial();
			mat->setVar("TU_Depth", 0);
			auto dirLight = g_GetCurrScene()->getDirectionLight();
			mat->setVar("sun_pos",  dirLight->dir());
			mat->setVar("TU_winSize", Engine::shared()->winSize());
			mat->use();
			
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
}
const int KERNEL_SIZE = 64;
static vec3 kernel[KERNEL_SIZE];
const int NOISE_SIZE = 16;
static vec3 noiseRandom[NOISE_SIZE];
static unsigned int noiseTexture;

static float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}
void Renderer::ssaoSetup()
{

    std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i)
    {
        vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
        sample = sample.normalized();
        sample *= randomFloats(generator);
        float scale = float(i) / KERNEL_SIZE;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
		kernel[i] = sample;
	}

    for (unsigned int i = 0; i < NOISE_SIZE; i++)
    {
        vec3 noise(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f); // rotate around z-axis (in tangent space)
		noiseRandom[i] = noise;
	}
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &noiseRandom[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Renderer::SSAOPass()
{
	//watch out the blend!!!!
	RenderBackEnd::shared()->disableFunction(RenderFlag::RenderFunction::AlphaBlend);
	auto currScene = g_GetCurrScene();
	m_gbuffer->bindForReading();
	m_gbuffer->bindDepth(1);
	m_gbuffer->bindForReadingGBuffer();
	//m_offScreenBuffer->bindForReadingGBuffer();
	m_ssaoBuffer1->bindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_postEffect->use();
	applyRenderSetting(m_postEffect);
	auto program = m_postEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	  
	program->setUniformInteger("TU_posBuffer",1);
	  
	program->setUniformInteger("TU_normalBuffer",2);
	  
	program->setUniformInteger("TU_GBUFFER4",3);
	  
	program->setUniformInteger("TU_Depth", 4);

	program->setUniform2Float("TU_winSize", Engine::shared()->winSize());
	auto cam = currScene->defaultCamera();
	program->setUniformMat4v("TU_viewProjectInverted", cam->getViewProjectionMatrix().inverted().data());
	program->setUniformMat4v("TU_Project", cam->projection().data());
	program->setUniformMat4v("TU_view", cam->getViewMatrix().data());
	
	program->setUniform3Floatv("gKernel",kernel,KERNEL_SIZE);
	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	program->setUniformInteger("gNoise",6);
	program->use();
	renderPrimitive(m_quad, m_postEffect, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::SSAOBlurVPass()
{
	m_ssaoBuffer1->bindRtToTexture(0, 0);
	m_ssaoBuffer2->bindForWriting();
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	m_blurVEffect->use();
	auto program = m_blurVEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniform2Float("TU_winSize", m_ssaoBuffer2->getFrameSize());
	renderPrimitive(m_quad, m_blurVEffect, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::SSAOBlurHPass()
{
	m_ssaoBuffer2->bindRtToTexture(0, 0);
	m_ssaoBuffer1->bindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_blurHEffect->use();
	auto program = m_blurHEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniform2Float("TU_winSize", m_ssaoBuffer1->getFrameSize());
	renderPrimitive(m_quad, m_blurHEffect, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::SSAOBlurCompossitPass()
{
	m_offScreenBuffer->bindRtToTexture(0, 0);
	m_ssaoBuffer1->bindRtToTexture(0,1);
	m_ssaoResultBuffer->bindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto program = m_ssaoCompositeEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniformInteger("TU_SSAOBuffer",1);
	renderPrimitive(m_quad, m_ssaoCompositeEffect, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::FogPass()
{
	if(m_fogEnable)
	{
		m_gbuffer->bindForReading();
		m_offScreenBuffer->bindForWriting();
		m_gbuffer->bindDepth(0);
		auto shader = m_fogEffect->getProgram();
		shader->use();
		m_fogEffect->use();
		shader->setUniformInteger("TU_Depth",0);
		shader->setUniform2Float("TU_winSize", Engine::shared()->winSize());
		applyRenderSetting(m_fogEffect);
		renderPrimitive(m_quad, m_fogEffect, RenderCommand::PrimitiveType::TRIANGLES);
	}
}

void Renderer::BloomBrightPass()
{
	m_ssaoResultBuffer->bindRtToTexture(0, 0);
	m_bloomBuffer1->bindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto program = m_bloomBrightPassEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniform2Float("TU_winSize", m_bloomBuffer1->getFrameSize());
	renderPrimitive(m_quad, m_bloomBrightPassEffect, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::BloomBlurVPass()
{
	m_bloomBuffer1->bindRtToTexture(0, 0);
	m_bloomBuffer2->bindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_BloomBlurVEffect->use();
	auto program = m_BloomBlurVEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniform2Float("TU_winSize", m_bloomBuffer2->getFrameSize());
	renderPrimitive(m_quad, m_BloomBlurVEffect, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::BloomBlurHPass()
{
	m_bloomBuffer2->bindRtToTexture(0, 0);
	m_bloomBuffer1->bindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_BloomBlurHEffect->use();
	auto program = m_BloomBlurHEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniform2Float("TU_winSize", m_bloomBuffer1->getFrameSize());
	renderPrimitive(m_quad, m_BloomBlurHEffect, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::BloomCompossitPass()
{
	m_ssaoResultBuffer->bindRtToTexture(0, 0);
	m_bloomBuffer1->bindRtToTexture(0, 1);
	m_bloomBuffer_half1->bindRtToTexture(0, 2);
	m_bloomBuffer_quad1->bindRtToTexture(0, 3);
	m_bloomBuffer_octave1->bindRtToTexture(0, 4);

	m_bloomResultBuffer->bindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_BloomCompositePassEffect->use();
	auto program = m_BloomCompositePassEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniformInteger("TU_BloomBuffer",1);
	program->setUniformInteger("TU_BloomBufferHalf",2);
	program->setUniformInteger("TU_BloomBufferQuad",3);
	program->setUniformInteger("TU_BloomBufferOctave",4);
	renderPrimitive(m_quad, m_BloomCompositePassEffect, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::directionalLightPass()
{
	auto currScene = g_GetCurrScene();

	applyRenderSetting(m_dirLightProgram);
	m_dirLightProgram->use();
	  
	auto program = m_dirLightProgram->getProgram();
	  
	program->use();
	  
	program->setUniformInteger("TU_colorBuffer",0);
	  
	program->setUniformInteger("TU_posBuffer",1);
	  
	program->setUniformInteger("TU_normalBuffer",2);
	  
	program->setUniformInteger("TU_GBUFFER4",3);
	  
	program->setUniformInteger("TU_Depth", 4);

	//For Shadow
	program->setUniformInteger("TU_ShadowMap[0]", 5);
	ShadowMap::shared()->getFBO(0)->BindForReading(5);

	program->setUniformInteger("TU_ShadowMap[1]", 6);
	ShadowMap::shared()->getFBO(1)->BindForReading(6);

	program->setUniformInteger("TU_ShadowMap[2]", 7);
	ShadowMap::shared()->getFBO(2)->BindForReading(7);

	
	//For IBL
	program->setUniformInteger("environmentMap", 8);
	RenderBackEnd::shared()->bindTexture2DAndUnit(8,m_envMap->handle(),m_envMap->getType());
	program->setUniformInteger("prefilterMap", 9);
	RenderBackEnd::shared()->bindTexture2DAndUnit(9,m_specularMap->handle(),m_specularMap->getType());

	//
	program->setUniform2Float("TU_winSize", Engine::shared()->winSize());

	
	program->setUniform3Float("TU_camPos", g_GetCurrScene()->defaultCamera()->getWorldPos());
	
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
	dirLight->tick(Engine::shared()->deltaTime());
	program->setUniform3Float("gDirectionalLight.direction",dirLight->dir());
	  
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

void Renderer::autoExposurePass()
{

	//pass 1
	m_ssaoResultBuffer->bindRtToTexture(0, 0);
	autoExposureList[0]->bindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_autoExposurePassEffect->use();
	auto program = m_autoExposurePassEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	renderPrimitive(m_quad, m_autoExposurePassEffect, RenderCommand::PrimitiveType::TRIANGLES);


	for(int i = 0; i < autoExposureList.size() - 1; i++)
	{
		//pass2
		autoExposureList[i]->bindRtToTexture(0, 0);
		autoExposureList[i + 1]->bindForWriting();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_autoExposurePassEffect->use();
		program = m_autoExposurePassEffect->getProgram();
		program->use();
		program->setUniformInteger("TU_colorBuffer",0);
		renderPrimitive(m_quad, m_autoExposurePassEffect, RenderCommand::PrimitiveType::TRIANGLES);
	}
	
}

void Renderer::AAPass()
{
	m_offScreenBuffer2->bindRtToTexture(0, 0);
	bindScreenForWriting();
	RenderBackEnd::shared()->setDepthTestEnable(false);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_FXAAEffect->use();
	auto program = m_FXAAEffect->getProgram();
	program = m_FXAAEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	renderPrimitive(m_quad, m_FXAAEffect, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::applyRenderSetting(Material * mat)
{
	RenderBackEnd::shared()->selfCheck();
	RenderBackEnd::shared()->setIsCullFace(mat->getIsCullFace());
	RenderBackEnd::shared()->setBlendFactor(mat->getFactorSrc(),
	mat->getFactorDst());
	if(mat->isIsEnableBlend())
	{
		RenderBackEnd::shared()->enableFunction(RenderFlag::RenderFunction::AlphaBlend);
	}else
	{
		RenderBackEnd::shared()->disableFunction(RenderFlag::RenderFunction::AlphaBlend);
	}
	
	RenderBackEnd::shared()->setDepthMaskWriteEnable(mat->isIsDepthWriteEnable());
	RenderBackEnd::shared()->setDepthTestEnable(mat->isIsDepthTestEnable());
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
	program->setUniformMat4v("TU_pMatrix", p.data());

	program->setUniformMat4v("TU_mMatrix", m.data());
	program->setUniformMat4v("TU_mMatrixInverted", m.inverted().data());
	program->setUniformMat4v("TU_normalMatrix", (m).inverted().transpose().data());
}

void Renderer::toneMappingPass()
{
	if(m_aaEnable)
	{
		m_offScreenBuffer2->bindForWriting();
		
	}
	else
	{
		bindScreenForWriting();
	}
	m_bloomResultBuffer->bindRtToTexture(0, 0);
	autoExposureList[autoExposureList.size() - 1]->bindRtToTexture(0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_ToneMappingPassEffect->use();
	auto program = m_ToneMappingPassEffect->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniformInteger("TU_AverageLuminance",1);
	if(m_aaEnable)
	{
		program->setUniform2Float("TU_winSize", m_offScreenBuffer2->getFrameSize());
	}
	else
	{
		program->setUniform2Float("TU_winSize", Engine::shared()->winSize());
	}

	renderPrimitive(m_quad, m_ToneMappingPassEffect, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::bindScreenForWriting()
{
	auto size = Engine::shared()->winSize();
	glViewport(0, 0, int(size.x), int(size.y));
	RenderBackEnd::shared()->selfCheck();
	RenderBackEnd::shared()->bindFrameBuffer(0);
}

void Renderer::copyToFrame(FrameBuffer* bufferSrc, FrameBuffer* bufferDst, Material * mat)
{
	bufferDst->bindForWriting();
	bufferSrc->bindRtToTexture(0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat->use();
	auto program = mat->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniform2Float("TU_winSize", bufferDst->getFrameSize());
	renderPrimitive(m_quad, mat, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::copyToScreen(FrameBuffer* bufferSrc, Material* mat)
{
	auto winSize = Engine::shared()->winSize();
	glViewport(0, 0, winSize.x, winSize.y);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	bufferSrc->bindRtToTexture(0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat->use();
	auto program = mat->getProgram();
	program->use();
	program->setUniformInteger("TU_colorBuffer",0);
	program->setUniform2Float("TU_winSize", winSize);
	renderPrimitive(m_quad, mat, RenderCommand::PrimitiveType::TRIANGLES);
}

void Renderer::setIBL(std::string diffuseMap, std::string specularMap, bool isCubeMap)
{
	//IBL
	if(isCubeMap)
	{
		m_envMap = TextureMgr::shared()->loadSingleCubeMap(diffuseMap);
		m_specularMap = TextureMgr::shared()->loadSingleCubeMap(specularMap);
		ShaderMgr::shared()->addMacro("CUBE_MAP_IBL", "1");
	} else 
	{
		m_envMap = TextureMgr::shared()->getByPath(diffuseMap, true);
		m_specularMap = TextureMgr::shared()->getByPath(specularMap, true);
	}
	m_envMap->setFilter(Texture::FilterType::LinearMipMapLinear, 1);
	m_envMap->setFilter(Texture::FilterType::Linear, 2);

	m_specularMap->setFilter(Texture::FilterType::LinearMipMapLinear, 1);
	m_specularMap->setFilter(Texture::FilterType::Linear, 2);
}

void Renderer::handleThumbNail()
{
	for(auto thumbnail : m_thumbNailList)
	{
		if(!thumbnail->isIsDone())
		{
			thumbnail->doSnapShot();
			thumbnail->setIsDone(true);
		}
	}
	//m_thumbNailList.clear();
}

bool Renderer::isShadowEnable() const
{
	return m_shadowEnable;
}

void Renderer::setShadowEnable(const bool shadowEnable)
{
	m_shadowEnable = shadowEnable;
}

bool Renderer::isSkyEnable() const
{
	return m_skyEnable;
}

void Renderer::setSkyEnable(const bool skyEnable)
{
	m_skyEnable = skyEnable;
}

bool Renderer::isFogEnable() const
{
	return m_fogEnable;
}

void Renderer::setFogEnable(const bool fogEnable)
{
	m_fogEnable = fogEnable;
}

bool Renderer::isSsaoEnable() const
{
	return m_ssaoEnable;
}

void Renderer::setSsaoEnable(const bool ssaoEnable)
{
	m_ssaoEnable = ssaoEnable;
}

bool Renderer::isBloomEnable() const
{
	return m_bloomEnable;
}

void Renderer::setBloomEnable(const bool bloomEnable)
{
	m_bloomEnable = bloomEnable;
}

bool Renderer::isHdrEnable() const
{
	return m_hdrEnable;
}

void Renderer::setHdrEnable(const bool hdrEnable)
{
	m_hdrEnable = hdrEnable;
}

bool Renderer::isAaEnable() const
{
	return m_aaEnable;
}

void Renderer::setAaEnable(const bool aaEnable)
{
	m_aaEnable = aaEnable;
}
} // namespace tzw

