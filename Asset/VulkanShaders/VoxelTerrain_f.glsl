layout(set = 0, binding = 1) uniform sampler2D GrassTex;
layout(set = 0, binding = 2) uniform sampler2D NormalTex;
layout(set = 0, binding = 3) uniform sampler2D RoughnessTex;


layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;

layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_position;
layout(location = 2) out vec4 out_normal;
layout(location = 3) out vec4 out_mix;
void main() 
{
	out_albedo = texture(GrassTex, v_texcoord) * fragColor;
	out_position = vec4(0, 0, 0, 1.0);
	out_normal = texture(NormalTex, v_texcoord);
	out_mix = vec4(texture(RoughnessTex, v_texcoord).r, 0, 0, 1.0);
}
