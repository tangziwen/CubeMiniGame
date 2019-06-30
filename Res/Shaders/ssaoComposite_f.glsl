#version 130

uniform sampler2D TU_colorBuffer;
uniform sampler2D TU_SSAOBuffer;
varying vec2 v_texcoord;
void main()
{
	vec4 color = texture2D(TU_colorBuffer, v_texcoord);
	vec4 ao = texture2D(TU_SSAOBuffer, v_texcoord);
	gl_FragColor = vec4(ao.xyz * color.xyz, 1.0);
}
