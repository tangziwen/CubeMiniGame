layout(set = 0, binding = 1) uniform sampler2D SpriteTexture;

layout(location = 0) out vec4 out_Color;
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;
layout(location = 2) in vec4 v_fragOverLayColor;
layout(location = 3) in vec4 v_extraInstanceInfo;
void main() 
{
	vec4 color = texture(SpriteTexture, v_texcoord);
	out_Color = vec4(mix(color.rgb, v_fragOverLayColor.rgb, v_fragOverLayColor.a).rgb,color.a * v_extraInstanceInfo.r);
}

