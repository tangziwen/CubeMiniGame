#version 330

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 TU_mvpMatrix;

attribute vec3 a_position;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;
varying vec3 pos;
varying vec3 sun_norm;
varying vec3 star_pos;
uniform vec3 sun_pos;
//! [0]
void main()
{
    // Calculate vertex position in screen space
	vec4 WVP_pos = TU_mvpMatrix * vec4(a_position,1.0);
	pos = a_position;
    gl_Position = WVP_pos.xyww;
    v_texcoord = a_texcoord;
	//Sun pos being a constant vector, we can normalize it in the vshader
    //and pass it to the fshader without having to re-normalize it
    sun_norm = normalize(-sun_pos);

    //And we compute an approximate star position using the special rotation matrix
    star_pos = normalize(a_position);//rot_stars * normalize(a_position);
}
//! [0]
