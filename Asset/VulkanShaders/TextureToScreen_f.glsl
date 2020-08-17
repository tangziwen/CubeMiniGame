layout(set = 0, binding = 1) uniform sampler2D RenderTarget1;
layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;

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
  out_Color = vec4(pow(ACES_Knarkowicz(texture(RenderTarget1, v_texcoord).rgb), vec3(1.0 / 2.2)), 1.0);//vec4( fragColor.rgb, fragColor.a);
}
