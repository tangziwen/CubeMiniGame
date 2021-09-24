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
float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
void main() 
{
	vec2 UV = vec2(gl_GlobalInvocationID.x / t_shaderUnifom.TU_OutSize.x, gl_GlobalInvocationID.y / t_shaderUnifom.TU_OutSize.y);
	ivec2 InPixelPos = ivec2(UV.x * t_shaderUnifom.TU_InSize.x, UV.y * t_shaderUnifom.TU_InSize.y);
	vec3 accum = imageLoad(inBuffer, InPixelPos).rgb * weight[0];
	for(int i = 1; i< 5; i ++)
	{
		accum += imageLoad(inBuffer, InPixelPos + ivec2(i, 0)).rgb * weight[i];
		accum += imageLoad(inBuffer, InPixelPos - ivec2(i, 0)).rgb * weight[i];
	}
	imageStore(targetBuffer, ivec2(gl_GlobalInvocationID.xy), vec4(accum, 1.0));
}