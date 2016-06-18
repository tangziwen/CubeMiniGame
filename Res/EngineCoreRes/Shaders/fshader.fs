#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform sampler2D texture_1;

varying vec2 v_texcoord;

//! [0]
void main()
{
    // Set fragment color from texture
    vec4 col = texture2D(texture_1,v_texcoord);

    gl_FragColor = col;
    
}
//! [0]

