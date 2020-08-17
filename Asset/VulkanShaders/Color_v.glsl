#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mvp;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 texcoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 v_texcoord;

void main() {
    gl_Position = ubo.mvp * vec4(inPosition, 1.0);
	
	v_texcoord = texcoord;
    fragColor = inColor;
}