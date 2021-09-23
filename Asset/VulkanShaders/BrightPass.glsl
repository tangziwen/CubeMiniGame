#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (local_size_x = 16, local_size_y = 16) in;
layout(set = 0, binding = 0, rgba16f) readonly uniform image2D inBuffer;
layout(set = 0, binding = 1, rgba16f) uniform image2D targetBuffer;

void main() 
{
	vec4 color = imageLoad(inBuffer, ivec2(gl_GlobalInvocationID.xy));
	float gray = color.r * 0.3 + color.g * 0.6 + color.b * 0.1;
	vec4 resultColor = vec4(0, 0, 0, 1.0);
	if(gray > 0.5)
	{
		resultColor = color;
	}
	imageStore(targetBuffer, ivec2(gl_GlobalInvocationID.xy), resultColor);
}