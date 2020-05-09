#include "RenderBackEnd.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Utility/log/Log.h"

namespace tzw {
RenderBackEnd * RenderBackEnd::m_instance = nullptr;

void RenderBackEnd::selfCheck()
{
	// if (!m_isCheckGL)
	// 	return;
	bool isBad = false;
	while(true)
	{
		auto errorCode = glGetError();
		if(errorCode == GL_NO_ERROR)
		{
			break;
		}
		tlogError("error %d",errorCode);
		isBad = true;
		exit(0);
	}
	if(isBad)
	{
		abort();
	}

}

void RenderBackEnd::setIsCheckGL(bool newVal)
{
	m_isCheckGL = newVal;
}

bool RenderBackEnd::isCheckGL()
{
	return m_isCheckGL;
}

std::string RenderBackEnd::getCurrVersion()
{
	return reinterpret_cast<const char *>(glGetString(GL_VERSION));
}

std::string RenderBackEnd::getShaderSupportVersion()
{
	return reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION ));
}

void RenderBackEnd::printFullDeviceInfo()
{

	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	auto shader_version = glGetString(GL_SHADING_LANGUAGE_VERSION );
	tlog("Vendor %s\n Renderer %s\n Support GL Version %s\n shader version %s\n", vendor, renderer, version, shader_version);
}

RenderBackEnd *RenderBackEnd::shared()
{
	if(!m_instance)
	{
		m_instance = new RenderBackEnd();

	}

	return m_instance;
}

void RenderBackEnd::initDevice()
{
	auto result = glewInit();
	if(result != GLEW_OK)
	{
		tlogError("opengl device init error!\n");
		exit(0);
	}

	// Set the background color
	glClearColor(0, 0, 0, 1);
	selfCheck();

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	selfCheck();
	// Enable back face culling
	glEnable(GL_CULL_FACE);
	selfCheck();

	// Enable alpha blending
	glEnable(GL_BLEND);
	selfCheck();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	selfCheck();
	m_curBlendSrc = RenderFlag::BlendingFactor::SrcAlpha;
	m_curBlendDst = RenderFlag::BlendingFactor::OneMinusSrcAlpha;

}

unsigned int RenderBackEnd::genBuffer()
{
	unsigned int handle;
	glGenBuffers(1,&handle);
	selfCheck();
	return handle;
}

void RenderBackEnd::deleteBuffer(unsigned index)
{
	glDeleteBuffers(1, &index);
}

void RenderBackEnd::bindBuffer(RenderFlag::BufferTarget target, unsigned int handle)
{
	switch(target)
	{
		case RenderFlag::BufferTarget::VertexBuffer:
			glBindBuffer(GL_ARRAY_BUFFER,handle);
			selfCheck();
		break;
		case RenderFlag::BufferTarget::IndexBuffer:
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,handle);
			selfCheck();
		break;
		default:

		break;
	}
}

void RenderBackEnd::submit(RenderFlag::BufferTarget target, unsigned int size, const void *data, RenderFlag::BufferStorageType storageType)
{
	switch(target)
	{
		case RenderFlag::BufferTarget::VertexBuffer:
			glBufferData(GL_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
			selfCheck();
		break;
		case RenderFlag::BufferTarget::IndexBuffer:
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
			selfCheck();
		break;
		default:
		break;
	}
}

void RenderBackEnd::resubmit(RenderFlag::BufferTarget target, unsigned offset, unsigned size, const void* data)
{
	switch(target)
	{
		case RenderFlag::BufferTarget::VertexBuffer:
			glBufferSubData(GL_ARRAY_BUFFER,offset, size,data);
			selfCheck();
		break;
		case RenderFlag::BufferTarget::IndexBuffer:
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size,data);
			selfCheck();
		break;
		default:
		break;
	}
}

void RenderBackEnd::activeTextureUnit(unsigned int id)
{
	glActiveTexture(GL_TEXTURE0 + id);
	selfCheck();
}

void RenderBackEnd::enableFunction(RenderFlag::RenderFunction state)
{
	switch(state)
	{
		case RenderFlag::RenderFunction::DepthTest:
			glEnable(GL_DEPTH_TEST);
			selfCheck();
		break;
		case RenderFlag::RenderFunction::AlphaBlend:
			glEnable(GL_BLEND);
			selfCheck();
		break;
	}
}

void RenderBackEnd::disableFunction(RenderFlag::RenderFunction state)
{
	switch(state)
	{
		case RenderFlag::RenderFunction::DepthTest:
			glDisable(GL_DEPTH_TEST);
			selfCheck();
		break;
		case RenderFlag::RenderFunction::AlphaBlend:
			glDisable(GL_BLEND);
			selfCheck();
		break;
	}
}

void RenderBackEnd::setTexMIN(unsigned int textureID, int param, RenderFlag::TextureType type)
{
	bindTexture2DAndUnit(0,textureID,type);
	switch (type) {
		case RenderFlag::TextureType::Texture2D:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
			selfCheck();
		break;
		case RenderFlag::TextureType::TextureCubeMap:
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, param);
			selfCheck();
		break;
		default:
		break;
	}
}

void RenderBackEnd::setTexMAG(unsigned int textureID, int param, RenderFlag::TextureType type)
{
	bindTexture2DAndUnit(0,textureID,type);
	switch (type) {
		case RenderFlag::TextureType::Texture2D:
			glBindTexture(GL_TEXTURE_2D,textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			selfCheck();
		break;
		case RenderFlag::TextureType::TextureCubeMap:
			glBindTexture(GL_TEXTURE_CUBE_MAP,textureID);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, param);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
			selfCheck();
		break;
		default:
		break;
	}
	
}

void RenderBackEnd::bindTexture2DAndUnit(unsigned int texUnitID, unsigned int textureID, RenderFlag::TextureType type)
{
	//    if (m_textureSlot[texUnitID] == textureID) return;
	m_textureSlot[texUnitID] = textureID;
	glActiveTexture(GL_TEXTURE0 + texUnitID);
	selfCheck();
	switch(type)
	{
		case RenderFlag::TextureType::Texture2D:
			glBindTexture(GL_TEXTURE_2D,textureID);
			selfCheck();
		break;
		case RenderFlag::TextureType::TextureCubeMap:
			glBindTexture(GL_TEXTURE_CUBE_MAP,textureID);
			selfCheck();
		break;
	}
}

void RenderBackEnd::drawElement(RenderFlag::IndicesType type, unsigned int size, const void * indicesAddress)
{
	switch(type)
	{
		case RenderFlag::IndicesType::Lines:
			glDrawElements(GL_LINES,size, GL_UNSIGNED_SHORT, indicesAddress);
			selfCheck();
		break;
		case RenderFlag::IndicesType::Triangles:
			selfCheck();
			glDrawElements(GL_TRIANGLES,size, GL_UNSIGNED_SHORT, indicesAddress);
			selfCheck();
		break;
		case RenderFlag::IndicesType::TriangleStrip:
			glDrawElements(GL_TRIANGLE_STRIP, size, GL_UNSIGNED_SHORT, indicesAddress);
			selfCheck();
		break;
		case RenderFlag::IndicesType::Patches:
			glDrawElements(GL_PATCHES, size, GL_UNSIGNED_SHORT, indicesAddress);
			selfCheck();
		break;
	}
}

void RenderBackEnd::drawElementInstanced(RenderFlag::IndicesType type, unsigned int size, const void * indicesAddress, int count)
{
	switch (type)
	{
	case RenderFlag::IndicesType::Triangles:
		
		glDrawElementsInstancedARB(GL_TRIANGLES, size, GL_UNSIGNED_SHORT, indicesAddress, count);
		selfCheck();
		break;
	case RenderFlag::IndicesType::Lines: break;
	case RenderFlag::IndicesType::TriangleStrip: break;
	case RenderFlag::IndicesType::Patches: break;
	default: ;
	}
}

void RenderBackEnd::setDepthTestMethod(const RenderFlag::DepthTestMethod &method)
{
	if(m_depthTestMethodCache == method) return;
	m_depthTestMethodCache = method;
	switch(method)
	{
		case RenderFlag::DepthTestMethod::Less:
			glDepthFunc(GL_LESS);
			selfCheck();
		break;
		case RenderFlag::DepthTestMethod::LessOrEqual:
			glDepthFunc(GL_LEQUAL);
			selfCheck();
		break;
	}
}

void RenderBackEnd::setTextureWarp(unsigned int textureID, RenderFlag::WarpAddress warpAddress, RenderFlag::TextureType type)
{
	bindTexture2DAndUnit(0,textureID,type);
	unsigned int flag = 0;
	switch (warpAddress) {
		case RenderFlag::WarpAddress::Clamp:
			flag = GL_CLAMP;
		break;
		case RenderFlag::WarpAddress::Repeat:
			flag = GL_REPEAT;
		break;
		case RenderFlag::WarpAddress::ClampToEdge:
			flag = GL_CLAMP_TO_EDGE;
		break;
		default:
		break;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, flag);
	selfCheck();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, flag);
	selfCheck();
}

void RenderBackEnd::bindFrameBuffer(unsigned int frameBufferID)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferID);
	selfCheck();
}

void RenderBackEnd::setBlendEnable(bool isEnable)
{
	if(isEnable)
	{
		glEnable(GL_BLEND);
	} else
	{
		glDisable(GL_BLEND);
	}
	selfCheck();
}

void RenderBackEnd::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderBackEnd::blitFramebuffer(int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1)
{
	glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	selfCheck();
}

void RenderBackEnd::setDepthTestEnable(bool isEnable)
{
	if(isEnable){
		glEnable(GL_DEPTH_TEST);
	}
	else{
		glDisable(GL_DEPTH_TEST);
	}
	selfCheck();
}

void RenderBackEnd::setBlendEquation(RenderFlag::BlendingEquation equation)
{
	switch(equation)
	{
		case RenderFlag::BlendingEquation::Add:
			glBlendEquation(GL_FUNC_ADD);
			selfCheck();
		break;
	}
}
static unsigned int getGLFactor(RenderFlag::BlendingFactor factor)
{
	switch(factor)
	{
		case RenderFlag::BlendingFactor::One:
		return GL_ONE;
		case RenderFlag::BlendingFactor::Zero:
		return GL_ZERO;
		case RenderFlag::BlendingFactor::SrcAlpha:
		return GL_SRC_ALPHA;
		case RenderFlag::BlendingFactor::OneMinusSrcAlpha:
		return GL_ONE_MINUS_SRC_ALPHA;
		case RenderFlag::BlendingFactor::ConstantAlpha:
		return GL_CONSTANT_ALPHA;
	}
}

void RenderBackEnd::setBlendFactor(RenderFlag::BlendingFactor factorSrc, RenderFlag::BlendingFactor factorDst)
{
	//if(m_curBlendSrc == factorSrc && m_curBlendDst == factorDst)
	//{
	//	return;
	//}
	glBlendFunc(getGLFactor(factorSrc),getGLFactor(factorDst));
	selfCheck();
}

void RenderBackEnd::setDepthMaskWriteEnable(bool isEnable)
{
	if(isEnable)
	{
		glDepthMask(GL_TRUE);
		selfCheck();
	}else
	{
		glDepthMask(GL_FALSE);
		selfCheck();
	}
}

void RenderBackEnd::genMipMap(unsigned int texUnitID)
{
	//generate Mip Map may faield
	glBindTexture(GL_TEXTURE_2D, texUnitID);
	selfCheck();
	glGenerateMipmap(GL_TEXTURE_2D);
	selfCheck();
}

void RenderBackEnd::setClearColor(float r, float g, float b)
{
	glClearColor(r, g, b, 1);
	selfCheck();
}

void RenderBackEnd::setIsCullFace(bool val)
{
	if (m_isCullFace != val)
	{
		if (val)
		{
			glEnable(GL_CULL_FACE);
			selfCheck();
			
		}
		else
		{
			glDisable(GL_CULL_FACE);
			selfCheck();
		}
	}
	m_isCullFace = val;
}

bool RenderBackEnd::getIsCullFace()
{
	return m_isCullFace;
}

void RenderBackEnd::deleteFramebuffers(unsigned count, unsigned * obj)
{
	glDeleteFramebuffers(count, obj);
	selfCheck();
}

void RenderBackEnd::DeleteTextures(unsigned count, unsigned * obj)
{
	glDeleteTextures(count, obj);
	selfCheck();
}

void RenderBackEnd::GenFramebuffers(unsigned count, unsigned * obj)
{
	glGenFramebuffers(count, obj);
	selfCheck();
}

void RenderBackEnd::GenTextures(unsigned count, unsigned * obj)
{
	glGenTextures(count, obj);
	selfCheck();
}

void RenderBackEnd::TexImage2D(unsigned target, int level, int internalformat, int width, int height, int border, unsigned format, unsigned type, const void * pixels)
{
	glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	selfCheck();
}

void RenderBackEnd::checkGL(int except_val)
{
	return;
	int errorCode = 0;
	bool isFuckedUp = false;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		tlogError("gl error %d\n", errorCode);
		if (errorCode == except_val)
			continue;
		isFuckedUp = true;
	}
	if (isFuckedUp)
	{
		assert(0);
	}
}

RenderBackEnd::RenderBackEnd(): m_isCullFace(false)
{
	memset(m_textureSlot, 0, sizeof(m_textureSlot));
	m_depthTestMethodCache = RenderFlag::DepthTestMethod::Less;
	m_isCheckGL = false;
}
} // namespace tzw

