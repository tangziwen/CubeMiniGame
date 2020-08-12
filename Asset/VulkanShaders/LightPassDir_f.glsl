layout(set = 0, binding = 1) uniform sampler2D RT_albedo;
layout(set = 0, binding = 2) uniform sampler2D RT_position;
layout(set = 0, binding = 3) uniform sampler2D RT_normal;
layout(set = 0, binding = 4) uniform sampler2D RT_mix;
layout(set = 0, binding = 5) uniform sampler2D RT_depth;
layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;
void main() 
{
	vec3 lightDir = vec3(-0.5, -1, 0);
	vec3 albedo = texture(RT_albedo, v_texcoord).rgb;
	vec3 normal = texture(RT_normal, v_texcoord).rgb;
	vec3 resultColor = albedo;// * clamp(dot(lightDir, normal.xyz), 0, 1);
	
	out_Color = vec4(resultColor, 1.0);
}
