

uniform sampler2D TU_colorBuffer;
varying vec2 v_texcoord;
uniform vec2 TU_winSize;
float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
void main()
{

	float dx = 1.0 / TU_winSize.x;
	float dy = 1.0 / TU_winSize.y;
	vec3 accum = texture2D(TU_colorBuffer, v_texcoord).rgb * weight[0];
	for(int i = 1; i< 5; i ++)
	{
		accum += texture2D(TU_colorBuffer, v_texcoord + vec2(0.0, i * dy)).rgb * weight[i];
		accum += texture2D(TU_colorBuffer, v_texcoord - vec2(0.0, i * dy)).rgb * weight[i];
	}
	gl_FragColor = vec4(accum, 1.0);
}