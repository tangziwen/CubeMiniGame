#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform sampler2D TU_tex1;
varying vec2 v_texcoord;
varying vec4 v_color;

//! [0]
void main()
{
    // Set fragment color from texture
    vec4 col = texture2D(TU_tex1,v_texcoord)* v_color;
    gl_FragColor = col * v_color;
}
//! [0]

