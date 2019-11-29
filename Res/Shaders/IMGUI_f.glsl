#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform sampler2D DiffuseMap;
varying vec2 v_texcoord;
varying vec4 v_color;

//! [0]
void main()
{
    // Set fragment color from texture
    vec4 col = texture2D(DiffuseMap,v_texcoord)* v_color;
    gl_FragColor = col;
}
//! [0]

