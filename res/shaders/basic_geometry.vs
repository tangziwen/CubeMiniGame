#version 300 es
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 g_MVP_matrix;

const int MAX_BONES = 100;
uniform mat4 g_MV_matrix;
uniform mat4 g_light_MVP_matrix;
uniform mat4 g_model_matrix;
uniform mat4 g_projection_matrix;
uniform mat4 g_bones[MAX_BONES];
uniform int  g_has_animation;

in vec3 a_position;
in vec2 a_texcoord;
in vec3 a_normal_line;
in ivec4 a_bone_ID;
in vec4 a_bone_weight;
in vec3 a_tangent;

out vec2 v_texcoord;
out vec3 v_normal_line;
out vec3 v_world_position;
out vec4 v_light_space_postion;
out vec3 v_tangent;
void main()
{
	vec4 actual_pos;
	if(g_has_animation == 1)
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

	v_tangent = (g_model_matrix * vec4(a_tangent, 0.0)).xyz;

	v_light_space_postion = g_light_MVP_matrix * actual_pos ; 

	gl_Position = g_MVP_matrix*actual_pos;
}
//! [0]
