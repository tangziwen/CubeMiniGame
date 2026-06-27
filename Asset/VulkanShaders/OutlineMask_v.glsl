#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObjectMat {
    vec4 TU_color;
} t_shaderUnifom;

layout(set = 1, binding = 0) uniform UniformBufferObject {
    mat4 mvp;
    mat4 wv;
    mat4 world;
    mat4 view;
    mat4 projection;
    vec4 outlineColor;
} t_ObjectUniform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec4 fragColor;

void main()
{
    gl_Position = t_ObjectUniform.mvp * vec4(inPosition, 1.0);
    fragColor = t_ObjectUniform.outlineColor;
}
