#version 300 es  
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

layout (location = 0) out vec3 DiffuseOut;   
layout (location = 1) out vec3 WorldPosOut;   
layout (location = 2) out vec3 NormalOut;   

varying vec3 v_texcoord;
varying vec3 v_normal_line;
varying vec3 v_world_position;

uniform samplerCube gCubemapTexture;        
void main()
{
	DiffuseOut = textureCube(gCubemapTexture, v_texcoord).xyz;
    WorldPosOut = vec3(0,0,0);
    NormalOut = vec3(0,0,0);
	return;
}

