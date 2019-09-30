#version 130

uniform sampler2D TU_colorBuffer;
varying vec2 v_texcoord;
uniform vec2 TU_winSize;
void main()
{
	gl_FragColor = texture2D(TU_colorBuffer, v_texcoord);
}