#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 mvp;
} t_ObjectUniform;

layout(set = 1, binding = 0) uniform UniformBufferObjectMat {
    vec4 TU_color;
} t_shaderUnifom;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 v_texcoord;

void main() {
    gl_Position = t_ObjectUniform.mvp * vec4(inPosition.xy, -0.5, 1.0);
	v_texcoord = texcoord;
    fragColor = inColor * t_shaderUnifom.TU_color;
}