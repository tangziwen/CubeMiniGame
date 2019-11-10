#version 130

uniform sampler2D TU_colorBuffer;
uniform sampler2D TU_AverageLuminance;
varying vec2 v_texcoord;

vec3 ACES_Knarkowicz( vec3 x )
{
	float a = 2.51;
	float b = 0.03;
	float c = 2.43;
	float d = 0.59;
	float s = 0.14;
	return (x*(a*x+b))/(x*(c*x+d)+s);
}


void main()
{
	float gamma = 2.2;
	vec4 color = texture2D(TU_colorBuffer, v_texcoord);
	float averageLuminace = texture2D(TU_AverageLuminance, vec2(0.5, 0.5)).r;
	float exposure = 0.18 / averageLuminace;
	vec3 hdrColor = color.xyz;// * exposure;
	// tone mapping
	vec3 result = ACES_Knarkowicz(hdrColor);
	// also gamma correct while we're at it
	result = pow(result, vec3(1.0 / gamma));
	gl_FragColor = vec4(result, 1.0);
}
