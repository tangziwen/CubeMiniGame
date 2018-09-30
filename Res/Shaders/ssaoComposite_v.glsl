#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;


varying vec2 v_texcoord;

void main()
{
	v_texcoord = a_texcoord;
    gl_Position = vec4(a_position,1.0);
}

