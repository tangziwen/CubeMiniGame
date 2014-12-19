#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 g_MVP_matrix;

attribute vec3 a_position;
void main()
{
	gl_Position = g_MVP_matrix * vec4(a_position,1.0);
}
