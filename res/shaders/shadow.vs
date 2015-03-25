#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 g_MVP_matrix;
const int MAX_BONES = 100;

uniform mat4 g_MV_matrix;
uniform mat4 g_model_matrix;
uniform mat4 g_projection_matrix;
uniform mat4 g_bones[MAX_BONES];
uniform int g_has_animation;

attribute vec3 a_position;
attribute vec2 a_texcoord;
attribute vec3 a_normal_line; 
attribute ivec4 a_bone_ID;
attribute vec4 a_bone_weight;



varying vec2 v_texcoord;
varying vec3 v_normal_line;
varying vec3 v_world_position;
void main()
{
	vec4 actual_pos;
	if(g_has_animation ==1)
	{
	mat4 BoneTransform;
    BoneTransform     = g_bones[a_bone_ID[0]]*a_bone_weight[0];
    BoneTransform     += g_bones[a_bone_ID[1]]*a_bone_weight[1];
    BoneTransform     += g_bones[a_bone_ID[2]]*a_bone_weight[2];
    BoneTransform     += g_bones[a_bone_ID[3]]*a_bone_weight[3];
	actual_pos = BoneTransform*vec4(a_position, 1.0);
	}else
	{
		actual_pos = vec4(a_position,1.0);
	}
    v_texcoord = a_texcoord;
	
	v_normal_line = (g_model_matrix * vec4(a_normal_line,0)).xyz ;

	v_world_position = (g_model_matrix * actual_pos).xyz;

	gl_Position = g_MVP_matrix * actual_pos ;
}
//! [0]
