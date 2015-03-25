#version 300 es
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

in vec3 v_texcoord;
in vec3 v_normal_line;
in vec3 v_world_position;

uniform samplerCube gCubemapTexture;        
void main()
{
	gl_FragColor = textureCube(gCubemapTexture, v_texcoord);  
	return;
}

