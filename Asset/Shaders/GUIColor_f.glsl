#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying vec2 v_texcoord;
varying vec4 v_color;
//! [0]
void main()
{
    gl_FragData[0] = v_color;
}
//! [0]

