#version 130

uniform sampler2D TU_colorBuffer;
uniform vec2 TU_winSize;
varying vec2 v_texcoord;
float getLuminace(vec3 color)
{
	return max(color.r * 0.3 + color.g* 0.59 + color.b * 0.11, 0.0);
}
void main()
{
	float texelX = 1.0 / TU_winSize.x;
	float texelY = 1.0 / TU_winSize.y;
	vec4 color = vec4(0, 0, 0, 1.0);
	color += texture2D(TU_colorBuffer, v_texcoord);
	color += texture2D(TU_colorBuffer, v_texcoord + vec2(texelX, 0));
	color += texture2D(TU_colorBuffer, v_texcoord + vec2(-texelX, 0));
	color += texture2D(TU_colorBuffer, v_texcoord + vec2(0, texelY));
	color += texture2D(TU_colorBuffer, v_texcoord + vec2(0, -texelY));
	color += texture2D(TU_colorBuffer, v_texcoord + vec2(texelX, texelY));
	color += texture2D(TU_colorBuffer, v_texcoord + vec2(texelX, -texelY));
	color += texture2D(TU_colorBuffer, v_texcoord + vec2(-texelX, -texelY));
	color += texture2D(TU_colorBuffer, v_texcoord + vec2(-texelX, texelY));
	color /= 9.0;
	gl_FragColor = vec4(vec3(getLuminace(color.rgb)), 1.0);
}
