#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	mat4 mvp;
	mat4 wv;
	mat4 TU_mMatrix;
	mat4 view;
	mat4 projection;
} t_ObjectUniform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 v_texcoord;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_tangent;
void main() {
    gl_Position = t_ObjectUniform.mvp * vec4(inPosition, 1.0);
	
	v_texcoord = texcoord;

	//hack for vulkan
	gl_Position.y = -gl_Position.y;
	gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
	fragColor = inColor;
	v_normal = (t_ObjectUniform.TU_mMatrix * vec4(inNormal,0.0)).xyz;
	v_tangent = (t_ObjectUniform.TU_mMatrix * vec4(inTangent,0.0)).xyz;
}