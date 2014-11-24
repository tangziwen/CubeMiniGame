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
attribute vec3 a_tangent;

varying vec2 v_texcoord;
varying vec3 v_normal_line;
varying vec3 world_position;
varying vec4 LightSpace_postion;
varying vec3 v_tangent;
void main()
{
	vec4 actual_pos;
	if(hasAnimation)
	{
	mat4 BoneTransform;
    BoneTransform     = gBones[BONE_ID[0]]*BONE_WEIGHT[0];
    BoneTransform     += gBones[BONE_ID[1]]*BONE_WEIGHT[1];
    BoneTransform     += gBones[BONE_ID[2]]*BONE_WEIGHT[2];
    BoneTransform     += gBones[BONE_ID[3]]*BONE_WEIGHT[3];
	actual_pos = BoneTransform*vec4(a_position, 1.0);
	}else
	{
		actual_pos = vec4(a_position,1.0);
	}

    v_texcoord = a_texcoord;
	
	v_normal_line = (ModelMatrix * vec4(a_normal_line,0)).xyz ;

	world_position = (ModelMatrix * actual_pos).xyz;

	v_tangent = (ModelMatrix * vec4(a_tangent, 0.0)).xyz;

	LightSpace_postion = LightMvpMatrix * actual_pos ; 

	gl_Position = MvpMatrix*actual_pos;
}
//! [0]
