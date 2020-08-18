layout(set = 0, binding = 1) uniform sampler2D DepthMap;
layout(set = 0, binding = 2) uniform sampler2D ColorMap;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;

layout(location = 0) out vec4 out_Color;
void main() 
{
	vec3 color = texture(ColorMap, v_texcoord).rgb;
	float depth = texture(DepthMap, v_texcoord).r;
	float dFactor = step(depth, 0.9999);
	out_Color =vec4(mix(vec3(135.0 / 255.0, 206.0 / 255.0, 235.0 / 255.0), color, vec3(dFactor)), 1.0);
}
