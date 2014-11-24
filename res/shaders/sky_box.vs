#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 MvpMatrix;

const int MAX_BONES = 100;
uniform mat4 modelViewMatrix;
uniform mat4 LightMvpMatrix;
uniform mat4 ModelMatrix;
uniform mat4 Projection;
uniform mat4 gBones[MAX_BONES];
uniform int hasAnimation;

attribute vec3 a_position;
attribute vec2 a_texcoord;
attribute vec3 a_normal_line;
attribute vec4 BONE_ID;
attribute vec4 BONE_WEIGHT;

varying vec3 v_texcoord;
varying vec3 world_position;
varying vec4 LightSpace_postion;
void main()
{
	

	world_position = (ModelMatrix * vec4(a_position,1.0)).xyz;
	
	vec4 MVP_Pos = MvpMatrix * vec4(a_position, 1.0);
	gl_Position = MVP_Pos.xyww;

	v_texcoord = a_position;
}
//! [0]
