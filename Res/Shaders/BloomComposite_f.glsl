

uniform sampler2D TU_colorBuffer;
uniform sampler2D TU_BloomBuffer;
uniform sampler2D TU_BloomBufferHalf;
uniform sampler2D TU_BloomBufferQuad;
uniform sampler2D TU_BloomBufferOctave;
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
	vec3 hdrColor = color.xyz + bloom.xyz * BloomStrength * 0.5 + bloomHalf.xyz * BloomStrength * 0.2 + bloomQuad.xyz * BloomStrength * 0.1 + bloomOctave.xyz * BloomStrength * 0.1;
	gl_FragColor = vec4(hdrColor, 1.0);
}
