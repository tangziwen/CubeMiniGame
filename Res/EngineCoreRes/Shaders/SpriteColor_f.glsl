#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform vec4 TU_color;
varying vec2 v_texcoord;

//! [0]
void main()
{
    gl_FragColor = TU_color;
}
//! [0]

