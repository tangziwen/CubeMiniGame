#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 g_MVP_matrix;

uniform mat4 g_MV_matrix;
uniform mat4 g_light_MVP_matrix;
uniform mat4 g_model_matrix;
uniform mat4 g_projection_matrix;
uniform vec3 g_eye_position;
uniform int  g_has_animation;

attribute vec3 a_position;
attribute vec2 a_texcoord;
attribute vec3 a_normal_line;
attribute vec4 a_bone_ID;
attribute vec4 a_bone_weight;
attribute vec3 a_tangent;

varying vec2 v_texcoord;
varying vec3 v_normal_line;
varying vec3 v_world_position;
varying vec4 v_light_space_postion;
varying vec3 v_tangent;
void main()
{
	vec4 actual_pos;

	actual_pos = vec4(a_position,1.0);

    v_texcoord = a_texcoord;
	
	v_normal_line = (g_model_matrix * vec4(a_normal_line,0)).xyz ;

	v_world_position = (g_model_matrix * actual_pos).xyz;

	v_tangent = (g_model_matrix * vec4(a_tangent, 0.0)).xyz;

	v_light_space_postion = g_light_MVP_matrix * actual_pos ; 

	gl_Position = g_MVP_matrix*actual_pos;
}
//! [0]
