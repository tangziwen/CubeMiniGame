#version 130
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


uniform samplerCube TU_tex1;
uniform sampler2D TU_Depth;

varying vec3 v_texcoord;
vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= vec2(800, 600);
	return tmp;
}
#define E 0.000001
//! [0]
void main()
{
	
	vec4 color = textureCube(TU_tex1, v_texcoord.stp);
	float depth = texture(TU_Depth, getScreenCoord()).r;
	if(depth >= (1.0 - E))
	{
		gl_FragColor = color;
	}else
	{
		discard;
	}
}
//! [0]

