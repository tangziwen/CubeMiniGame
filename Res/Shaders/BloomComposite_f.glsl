#version 130

uniform sampler2D TU_colorBuffer;
uniform sampler2D TU_BloomBuffer;
uniform sampler2D TU_BloomBufferHalf;
uniform sampler2D TU_BloomBufferQuad;
uniform sampler2D TU_BloomBufferOctave;
uniform sampler2D TU_AverageLuminance;
uniform float BloomStrength;
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
	float gamma = 2.0;
	vec4 color = texture2D(TU_colorBuffer, v_texcoord);
	vec4 bloom = texture2D(TU_BloomBuffer, v_texcoord);
	vec4 bloomHalf = texture2D(TU_BloomBufferHalf, v_texcoord);
	vec4 bloomQuad = texture2D(TU_BloomBufferQuad, v_texcoord);
	vec4 bloomOctave = texture2D(TU_BloomBufferOctave, v_texcoord);
	float averageLuminace = texture2D(TU_AverageLuminance, vec2(0.5, 0.5)).r;

	float exposure = 0.18 / averageLuminace;
	vec3 hdrColor = color.xyz * exposure + bloom.xyz * BloomStrength + bloomHalf.xyz * BloomStrength + bloomQuad.xyz * BloomStrength + bloomOctave.xyz * BloomStrength;
	vec3 result = ACES_Knarkowicz(hdrColor);
	// tone mapping

	// also gamma correct while we're at it
	result = pow(result, vec3(1.0 / gamma));

	gl_FragColor = vec4(result, 1.0);
}
