#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 g_MVP_matrix;

uniform mat4 g_MV_matrix;
uniform mat4 g_model_matrix;
uniform mat4 g_projection_matrix;

attribute vec3 a_position;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;
void main()
{
    v_texcoord = a_texcoord;
	gl_Position = g_MVP_matrix*vec4(a_position,1.0);
}
//! [0]
