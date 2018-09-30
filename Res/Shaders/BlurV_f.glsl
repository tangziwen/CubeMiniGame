#version 130

uniform sampler2D TU_colorBuffer;
varying vec2 v_texcoord;
uniform vec2 TU_winSize;
int sample = 4;
void main()
{

	float dx = 1.0 / TU_winSize.x;
	float dy = 1.0 / TU_winSize.x;
	float accum = 0.0;
	for(int i = - sample / 2; i< sample / 2; i ++)
	{
		accum += texture2D(TU_colorBuffer, v_texcoord + vec2(0.0, i * dy)).r;
	}
	float result = accum / float(sample);
	gl_FragColor = vec4(result);
	//gl_FragColor = vec4(texture2D(TU_colorBuffer, v_texcoord));
}
