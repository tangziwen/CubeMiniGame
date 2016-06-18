#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform samplerCube TU_tex1;

varying vec3 v_texcoord;

//! [0]
void main()
{
    // Set fragment color from texture
    vec4 col = texture(TU_tex1,v_texcoord);

    gl_FragColor = vec4(col.xyz,1.0);
    
}
//! [0]

