 420
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


layout(binding = 0)uniform samplerCube DiffuseMap;
layout(binding = 1)uniform sampler2D TU_Depth;
uniform vec2 TU_winSize;
varying vec3 v_texcoord;
vec2 getScreenCoord()
{
	vec2 tmp = gl_FragCoord.xy;
	tmp /= TU_winSize;
	return tmp;
}
#define E 0.000001
//! [0]
void main()
{
	
	vec4 color = texture(DiffuseMap, v_texcoord.stp);
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

