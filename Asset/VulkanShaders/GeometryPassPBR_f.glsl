layout(set = 0, binding = 1) uniform sampler2D DiffuseMap;
layout(set = 0, binding = 2) uniform sampler2D MetallicMap;
layout(set = 0, binding = 3) uniform sampler2D RoughnessMap;
layout(set = 0, binding = 4) uniform sampler2D NormalMap;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;
layout(location = 2) in vec3 v_normal;
layout(location = 3) in vec3 v_tangent;
layout(location = 4) in vec3 v_color;


layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_position;
layout(location = 2) out vec4 out_normal;
layout(location = 3) out vec4 out_mix;

vec3 CalcBumpedNormal(vec3 inputNormal)
{
	vec3 Normal = normalize(inputNormal);
	vec3 Tangent = normalize(v_tangent.xyz);
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	vec3 Bitangent = cross(Tangent, Normal);
	vec3 BumpMapNormal = texture(NormalMap, v_texcoord).xyz;
	BumpMapNormal.y = 1.0 - BumpMapNormal.y;//we use directx style normal map. 1.0 - Green
	BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
	vec3 NewNormal;
	mat3 TBN = mat3(Tangent, Bitangent, Normal);
	NewNormal = TBN * BumpMapNormal;

	NewNormal = normalize(NewNormal);
	return NewNormal;	
}

void main() 
{
	vec4 col = texture(DiffuseMap, v_texcoord);
	col.rgb = pow(col.rgb, vec3(2.2)) * fragColor.rgb;
	col.rgb *= v_color;
	out_albedo = col;
	out_position = vec4(0, 0, 0, 1.0);
	out_normal = vec4(CalcBumpedNormal(v_normal),1.0);
	out_mix = vec4(texture(RoughnessMap, v_texcoord).r, texture(MetallicMap, v_texcoord).r, 0, 1.0);
}
