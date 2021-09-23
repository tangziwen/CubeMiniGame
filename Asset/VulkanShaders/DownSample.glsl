#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (local_size_x = 16, local_size_y = 16) in;
layout(set = 0, binding = 1, rgba16f) readonly uniform image2D inBuffer;
layout(set = 0, binding = 2, rgba16f) uniform image2D targetBuffer;

layout(set = 0, binding = 0) uniform UniformBufferObjectMat 
{
	vec2 TU_InSize;
	vec2 TU_OutSize;
} t_shaderUnifom;

void main() 
{
	vec2 UV = vec2(gl_GlobalInvocationID.x / t_shaderUnifom.TU_OutSize.x, gl_GlobalInvocationID.y / t_shaderUnifom.TU_OutSize.y);
	ivec2 InPixelPos = ivec2(UV.x * t_shaderUnifom.TU_InSize.x, UV.y * t_shaderUnifom.TU_InSize.y);
	vec4 color = imageLoad(inBuffer, InPixelPos);
	imageStore(targetBuffer, ivec2(gl_GlobalInvocationID.xy), color);
}