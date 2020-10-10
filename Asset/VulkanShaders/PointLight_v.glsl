#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform UniformBufferObject {
	mat4 mvp;
	vec4 LightPos;
	vec4 LightColor;
} t_ObjectUniform;
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 texcoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 v_texcoord;
layout(location = 2) out vec3 v_position;

void main() {
	gl_Position = t_ObjectUniform.mvp * vec4(inPosition, 1.0);
	v_position = inPosition;
	v_texcoord = texcoord;
	fragColor = inColor;
}