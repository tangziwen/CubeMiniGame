#version 330

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 TU_mvpMatrix;
attribute vec3 a_position;
attribute vec2 a_texcoord;

varying vec3 v_texcoord;

//! [0]
void main()
{
    // Calculate vertex position in screen space
	vec4 WVP_pos = TU_mvpMatrix * vec4(a_position,1.0);
    gl_Position = WVP_pos.xyww;
    v_texcoord = a_position;
}
//! [0]
