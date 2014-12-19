/*
        Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SHADOWMAPFBO_H
#define	SHADOWMAPFBO_H
#include <qopenglfunctions.h>
#include "shader/shader_program.h"
class ShadowMapFBO : protected QOpenGLFunctions
{
public:
    ShadowMapFBO();

    ~ShadowMapFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading(GLenum TextureUnit);
    GLuint getShadowMap();
    void applyShadowMapTexture(ShaderProgram * shader_program, int sampler_ID);
private:

    GLuint m_fbo;
    GLuint m_shadowMap;
};

#endif	/* SHADOWMAPFBO_H */

