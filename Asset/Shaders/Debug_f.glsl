#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform sampler2D TU_tex0;
uniform vec4 TU_color;
varying vec2 v_texcoord;
varying vec3 v_normal;

vec3 light = vec3(-0.5,-0.2,-0.5);

//! [0]
void main()
{	
	float factor = min(dot(light,v_normal),0.0)*0.5 + 0.5;
	vec3 color = texture2D(TU_tex0,v_texcoord);
    // Set fragment color from texture
    gl_FragColor =  vec4(color* factor,1.0);
}
//! [0]

