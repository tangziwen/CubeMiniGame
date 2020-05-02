#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 TU_mvpMatrix;
attribute vec3 a_position;
attribute vec2 a_texcoord;
attribute vec4 a_color;
varying vec2 v_texcoord;
varying vec4 v_color;
//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = TU_mvpMatrix * vec4(a_position,1.0);

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
	v_color = a_color;
}
//! [0]
