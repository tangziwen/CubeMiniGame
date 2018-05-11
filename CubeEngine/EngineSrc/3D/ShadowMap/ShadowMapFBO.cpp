#include "ShadowMapFBO.h"
#include "BackEnd/RenderBackEnd.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
namespace tzw
{
	ShadowMapFBO::ShadowMapFBO()
	{
		m_fbo = 0;
		m_shadowMap = 0;
		m_w = 0;
		m_h = 0;
	}

	ShadowMapFBO::~ShadowMapFBO()
	{
		if (m_fbo != 0) {
			RenderBackEnd::shared()->deleteFramebuffers(1, &m_fbo);
		}

		if (m_shadowMap != 0) {
			RenderBackEnd::shared()->DeleteTextures(1, &m_shadowMap);
		}
	}

	bool ShadowMapFBO::Init(unsigned int w, unsigned int h)
	{
		m_w = w;
		m_h = h;
		// Create the FBO
		glGenFramebuffers(1, &m_fbo);

		// Create the depth buffer
		glGenTextures(1, &m_shadowMap);
		glBindTexture(GL_TEXTURE_2D, m_shadowMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);

		// Disable writes to the color buffer
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error, status: 0x%x\n", Status);
			return false;
		}
		return true;
	}

	void ShadowMapFBO::BindForWriting()
	{
		glViewport(0, 0, m_w, m_h);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
		GLenum Status = glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE) 
		{
			printf("FB error, status: 0x%x\n", Status);
		}
	}


	void ShadowMapFBO::BindForReading(unsigned TextureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + TextureUnit);
		glBindTexture(GL_TEXTURE_2D, m_shadowMap);
	}
}