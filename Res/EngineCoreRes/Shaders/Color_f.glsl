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
    gl_FragColor = vec4(1.0,0.0,0.0,1.0);
}
//! [0]

