layout(set = 0, binding = 1) uniform sampler2D DiffuseMap;

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec3 v_tangent;

void main() 
{
	vec4 col = texture(DiffuseMap, v_texcoord);
	col.rgb = pow(col.rgb, vec3(2.2)) * fragColor.rgb;
	out_color = col;

}
