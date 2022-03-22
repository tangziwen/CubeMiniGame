#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform UniformBufferObject {
	mat4 mvp;
	mat4 wv;
	mat4 TU_mMatrix;
	mat4 view;
	mat4 projection;
} t_ObjectUniform;

layout(set = 0, binding = 0) uniform UniformBufferObjectMat {
    vec4 TU_color;
} t_shaderUnifom;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;

#ifdef FLAG_EnableInstanced
layout(location = 8) in mat4 a_instance_offset;
layout(location = 12) in vec4 a_instance_offset2;
#endif

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 v_texcoord;
layout(location = 2) out vec3 v_normal;
layout(location = 3) out vec3 v_tangent;
void main() {
	
	
#ifdef FLAG_EnableInstanced
	mat4 modelMatrix = a_instance_offset;
	gl_Position = t_ObjectUniform.mvp * modelMatrix * vec4(inPosition,1.0);
	v_normal = vec3(0, 1, 0);//(t_ObjectUniform.TU_mMatrix * modelMatrix* vec4(inNormal,0.0)).xyz;
	v_tangent = (t_ObjectUniform.TU_mMatrix * modelMatrix * vec4(inTangent,0.0)).xyz;
#else
    gl_Position = t_ObjectUniform.mvp * vec4(inPosition, 1.0);
	v_normal = (t_ObjectUniform.TU_mMatrix * vec4(inNormal,0.0)).xyz;
	v_tangent = (t_ObjectUniform.TU_mMatrix * vec4(inTangent,0.0)).xyz;
#endif
	v_texcoord = texcoord;
	fragColor = inColor;
}